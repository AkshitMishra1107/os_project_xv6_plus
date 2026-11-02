#include "kernel/types.h"
#include "user/user.h"
#include "kernel/dbg.h"

int
main(void)
{
  int pid = fork();
  if(pid == 0){
    dbg_traceme();
    char *args[] = { "echo", "hello from the target", 0 };
    exec("echo", args);
    exit(1);
  }

  int why = dbg_wait(pid);
  struct dbgregs r;
  uchar insn[4];
  dbg_getregs(pid, &r);
  dbg_peek(pid, r.pc, insn, 4);
  printf("dbgmini: stopped, reason %d, pc %p, sp %p\n", why, (void*)r.pc, (void*)r.sp);
  printf("dbgmini: bytes at pc: %x %x %x %x\n", insn[0], insn[1], insn[2], insn[3]);

  dbg_cont(pid);
  why = dbg_wait(pid);
  printf("dbgmini: final reason %d (3 means exited)\n", why);
  wait(0);
  exit(0);
}
