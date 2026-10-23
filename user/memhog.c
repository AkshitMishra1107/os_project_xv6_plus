#include "kernel/types.h"
#include "user/user.h"
#include "kernel/memstat.h"

#define PAGE 4096

static void
run_workload(int mb)
{
  int npages = mb * 256;
  char *base = sbrk(mb * 1024 * 1024);

  if(base == (char*)-1){
    printf("memhog: sbrk failed\n");
    exit(1);
  }

  for(int i = 0; i < npages; i++){
    base[i * PAGE] = i % 251;
    base[i * PAGE + PAGE - 1] = i % 241;
  }

  int bad = 0;

  for(int i = 0; i < npages; i++){
    if(base[i * PAGE] != (char)(i % 251) ||
       base[i * PAGE + PAGE - 1] != (char)(i % 241))
      bad++;
  }

  struct memstat ms;

  if(memstat(&ms) < 0){
    printf("memhog: memstat failed\n");
    exit(1);
  }

  printf("memhog: %d MB, %d pages, %d bad, "
         "swap outs %d, swap ins %d\n",
         mb, npages, bad,
         (int)ms.swap_outs, (int)ms.swap_ins);

  exit(bad ? 1 : 0);
}

int
main(int argc, char *argv[])
{
  int mb = 12;

  if(argc > 1)
    mb = atoi(argv[1]);

  if(mb < 1){
    printf("usage: memhog [positive MB]\n");
    exit(1);
  }

  printf("memhog: starting %d MB workload\n", mb);

  int pid = fork();

  if(pid < 0){
    printf("memhog: fork failed\n");
    exit(1);
  }

  if(pid == 0)
    run_workload(mb);

  int status = -2;

  if(wait(&status) < 0){
    printf("memhog: wait failed\n");
    exit(1);
  }

  printf("memhog: child exit status %d\n", status);
  exit(status == 0 ? 0 : 1);
}
