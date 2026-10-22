#include "kernel/types.h"
#include "user/user.h"

#define PAGE 4096

int
main(void)
{
  int mb = 20;
  int npages = mb * 256;
  char *base = sbrk(mb * 1024 * 1024);

  if(base == (char*)-1){
    printf("forkbench: sbrk failed\n");
    exit(1);
  }

  for(int i = 0; i < npages; i++)
    base[i * PAGE] = 1;

  int start = uptime();

  for(int n = 0; n < 50; n++){
    int pid = fork();

    if(pid < 0){
      printf("forkbench: fork failed\n");
      exit(1);
    }

    if(pid == 0)
      exit(0);

    int status;
    if(wait(&status) < 0 || status != 0){
      printf("forkbench: child failed\n");
      exit(1);
    }
  }

  printf("forkbench: 50 forks of a %d MB process took %d ticks\n",
         mb, uptime() - start);

  exit(0);
}
