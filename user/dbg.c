#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/elf.h"
#include "kernel/dbg.h"
#include "user/user.h"

#define MAXFUNC 256
#define MAXBP   8

// ELF64 section header and symbol. xv6's elf.h only has the file header.
struct shdr {
  uint32 name;
  uint32 type;
  uint64 flags;
  uint64 addr;
  uint64 offset;
  uint64 size;
  uint32 link;
  uint32 info;
  uint64 addralign;
  uint64 entsize;
};

struct sym {
  uint32 name;
  uchar info;
  uchar other;
  ushort shndx;
  uint64 value;
  uint64 size;
};

struct func {
  uint64 addr;
  uint64 size;
  char *name;
};

struct bp {
  uint64 addr;
  int len;              // 2 or 4 bytes
  uchar orig[4];        // the instruction we replaced
};

static struct func funcs[MAXFUNC];
static int nfuncs;
static struct bp bps[MAXBP];
static int nbps;
static int pid;

// ---- symbols ------------------------------------------------------------------

static int
load_symbols(char *path)
{
  struct stat st;
  int fd = open(path, O_RDONLY);

  if(fd < 0 || fstat(fd, &st) < 0){
    printf("dbg: cannot open %s\n", path);
    return -1;
  }
  char *img = malloc(st.size);
  if(img == 0){
    printf("dbg: out of memory\n");
    return -1;
  }
  uint64 got = 0;
  int n;
  while(got < st.size && (n = read(fd, img + got, st.size - got)) > 0)
    got += n;
  close(fd);
  if(got != st.size){
    printf("dbg: short read\n");
    return -1;
  }

  struct elfhdr *eh = (struct elfhdr *)img;
  if(eh->magic != ELF_MAGIC){
    printf("dbg: %s is not an ELF file\n", path);
    return -1;
  }
  struct shdr *sh = (struct shdr *)(img + eh->shoff);
  for(int i = 0; i < eh->shnum; i++){
    if(sh[i].type != 2)                       // 2 = SHT_SYMTAB
      continue;
    struct sym *syms = (struct sym *)(img + sh[i].offset);
    int nsyms = sh[i].size / sizeof(struct sym);
    char *strtab = img + sh[sh[i].link].offset;
    for(int j = 0; j < nsyms && nfuncs < MAXFUNC; j++){
      if((syms[j].info & 0xf) != 2 || syms[j].shndx == 0)   // 2 = STT_FUNC, 0 = SHN_UNDEF
        continue;
      funcs[nfuncs].addr = syms[j].value;
      funcs[nfuncs].size = syms[j].size;
      funcs[nfuncs].name = strtab + syms[j].name;
      nfuncs++;
    }
  }
  return 0;                                    // img stays allocated: names point into it
}

static struct func *
func_by_name(char *name)
{
  for(int i = 0; i < nfuncs; i++)
    if(strcmp(funcs[i].name, name) == 0)
      return &funcs[i];
  return 0;
}

static struct func *
func_by_addr(uint64 addr)
{
  for(int i = 0; i < nfuncs; i++)
    if(addr >= funcs[i].addr && addr < funcs[i].addr + funcs[i].size)
      return &funcs[i];
  return 0;
}

// ---- instructions -------------------------------------------------------------

static int
insn_len(uchar *b)                            // b points at the first byte
{
  return (b[0] & 3) == 3 ? 4 : 2;
}

// Is this a jump or a branch? We cannot step over those with the simple method.
static int
is_control_flow(uchar *b, int len)
{
  if(len == 4){
    uint op = b[0] & 0x7f;
    return op == 0x63 || op == 0x6f || op == 0x67;    // branch, jal, jalr
  }
  uint h = b[0] | (b[1] << 8);
  uint quadrant = h & 3;
  uint funct3 = h >> 13;
  if(quadrant == 1 && funct3 >= 5)                    // c.j, c.beqz, c.bnez
    return 1;
  if(quadrant == 2 && funct3 == 4 && (h & 0x7c) == 0) // c.jr, c.jalr, c.ebreak
    return 1;
  return 0;
}

static void
write_ebreak(uint64 addr, int len)
{
  uchar c_ebreak[2] = { 0x02, 0x90 };               // c.ebreak = 0x9002
  uchar ebreak[4]   = { 0x73, 0x00, 0x10, 0x00 };   // ebreak   = 0x00100073
  dbg_poke(pid, addr, len == 2 ? (void*)c_ebreak : (void*)ebreak, len);
}

// ---- breakpoints --------------------------------------------------------------

static struct bp *
bp_at(uint64 addr)
{
  for(int i = 0; i < nbps; i++)
    if(bps[i].addr == addr)
      return &bps[i];
  return 0;
}

static void
set_breakpoint(char *name)
{
  struct func *f = func_by_name(name);
  uchar b[4];

  if(f == 0){
    printf("no function named %s\n", name);
    return;
  }
  if(bp_at(f->addr)){
    printf("a breakpoint is already set at %s\n", name);
    return;
  }
  if(nbps == MAXBP){
    printf("too many breakpoints\n");
    return;
  }
  if(dbg_peek(pid, f->addr, b, 4) < 0){
    printf("cannot read the program's memory\n");
    return;
  }
  int len = insn_len(b);
  if(is_control_flow(b, len)){
    printf("cannot break at %s: its first instruction is a jump or branch\n", name);
    return;
  }
  struct bp *p = &bps[nbps++];
  p->addr = f->addr;
  p->len = len;
  memmove(p->orig, b, len);
  write_ebreak(p->addr, p->len);
  printf("breakpoint %d at %p (%s)\n", nbps, (void*)f->addr, f->name);
}

static void
list_breakpoints(void)
{
  for(int i = 0; i < nbps; i++){
    struct func *f = func_by_addr(bps[i].addr);
    printf("%d  %s  (%p)\n", i + 1, f ? f->name : "?", (void*)bps[i].addr);
  }
  if(nbps == 0)
    printf("no breakpoints\n");
}

// Resume the program. Returns DBG_BREAK if it stopped again, DBG_EXITED if it ended.
static int
resume(void)
{
  struct dbgregs r;

  if(dbg_getregs(pid, &r) < 0)
    return DBG_EXITED;
  struct bp *b = bp_at(r.pc);
  if(b){
    // Step over the breakpoint: put the real instruction back, plant a
    // temporary breakpoint right after it, run that one instruction, then
    // put both back the way they were.
    uchar after[4];
    uint64 next = b->addr + b->len;
    dbg_poke(pid, b->addr, b->orig, b->len);
    dbg_peek(pid, next, after, 4);
    int alen = insn_len(after);
    write_ebreak(next, alen);
    dbg_cont(pid);
    int why = dbg_wait(pid);
    if(why != DBG_BREAK)
      return DBG_EXITED;
    dbg_poke(pid, next, after, alen);         // remove the temporary breakpoint
    write_ebreak(b->addr, b->len);            // restore the real breakpoint
  }
  dbg_cont(pid);
  return dbg_wait(pid);
}

// ---- looking at the stopped program ----------------------------------------------

static void
report_stop(void)
{
  struct dbgregs r;

  dbg_getregs(pid, &r);
  struct bp *b = bp_at(r.pc);
  struct func *f = func_by_addr(r.pc);
  if(b)
    printf("Breakpoint %d, %s (%p)\n", (int)(b - bps) + 1, f ? f->name : "?", (void*)r.pc);
  else
    printf("stopped at %p\n", (void*)r.pc);
}

static void
print_frame(int n, uint64 addr)
{
  struct func *f = func_by_addr(addr);

  if(f)
    printf("#%d  %s + %d  (%p)\n", n, f->name, (int)(addr - f->addr), (void*)addr);
  else
    printf("#%d  ?  (%p)\n", n, (void*)addr);
}

static void
backtrace(void)
{
  struct dbgregs r;
  uint64 fp, floor, ra, next_fp;
  int n = 0;

  if(dbg_getregs(pid, &r) < 0)
    return;
  print_frame(n++, r.pc);
  print_frame(n++, r.ra);        // at a function's first instruction the caller is in ra
  fp = r.fp;
  floor = r.sp;
  while(n < 16){
    if(fp == 0 || fp < floor)
      break;                     // frame pointers must climb
    if(dbg_peek(pid, fp - 8, &ra, 8) < 0 || dbg_peek(pid, fp - 16, &next_fp, 8) < 0)
      break;
    if(func_by_addr(ra) == 0)
      break;                     // not code we know: end of the chain
    print_frame(n++, ra);
    floor = fp + 1;
    fp = next_fp;
  }
}

static void
print_regs(void)
{
  struct dbgregs r;

  if(dbg_getregs(pid, &r) < 0){
    printf("cannot read registers\n");
    return;
  }
  printf("pc %p  ra %p\nsp %p  fp %p\n", (void*)r.pc, (void*)r.ra, (void*)r.sp, (void*)r.fp);
  printf("a0 %p  a1 %p  a2 %p  a3 %p\n", (void*)r.a0, (void*)r.a1, (void*)r.a2, (void*)r.a3);
  printf("a4 %p  a5 %p  a6 %p  a7 %p\n", (void*)r.a4, (void*)r.a5, (void*)r.a6, (void*)r.a7);
}

// ---- main ---------------------------------------------------------------------

int
main(int argc, char *argv[])
{
  char line[100];
  int exited = 0;

  if(argc < 2){
    printf("usage: dbg program [args]\n");
    exit(1);
  }
  if(load_symbols(argv[1]) < 0)
    exit(1);

  pid = fork();
  if(pid < 0){
    printf("dbg: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    dbg_traceme();                            // let my parent control me
    exec(argv[1], &argv[1]);
    printf("dbg: exec %s failed\n", argv[1]);
    exit(1);
  }
  if(dbg_wait(pid) != DBG_EXEC){
    printf("dbg: the program did not start\n");
    exit(1);
  }
  printf("dbg: %s loaded, %d functions. Type help.\n", argv[1], nfuncs);

  for(;;){
    printf("(dbg) ");
    gets(line, sizeof(line));
    if(line[0] == 0)
      break;                                  // end of input
    line[strlen(line) - 1] = 0;               // drop the newline
    char *arg = strchr(line, ' ');
    if(arg){
      *arg++ = 0;
      while(*arg == ' ')
        arg++;
    }

    if(strcmp(line, "quit") == 0 || strcmp(line, "q") == 0)
      break;
    else if(strcmp(line, "help") == 0 || strcmp(line, "h") == 0)
      printf("break FUNC | run | continue | bt | regs | info | quit\n");
    else if(exited)
      printf("the program has exited: type quit\n");
    else if((strcmp(line, "break") == 0 || strcmp(line, "b") == 0) && arg)
      set_breakpoint(arg);
    else if(strcmp(line, "run") == 0 || strcmp(line, "r") == 0 ||
            strcmp(line, "continue") == 0 || strcmp(line, "c") == 0){
      if(resume() == DBG_BREAK){
        report_stop();
      } else {
        printf("program exited\n");
        wait(0);
        exited = 1;
      }
    }
    else if(strcmp(line, "bt") == 0)
      backtrace();
    else if(strcmp(line, "regs") == 0)
      print_regs();
    else if(strcmp(line, "info") == 0)
      list_breakpoints();
    else
      printf("unknown command: type help\n");
  }

  if(!exited){
    kill(pid);                                // the program is frozen: kill it,
    dbg_cont(pid);                            // let it notice, and collect it
    wait(0);
  }
  exit(0);
}
