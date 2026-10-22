#include "kernel/types.h"
#include "user/user.h"

#define PAGE 4096

int
main(void)
{
  int mb = 80;
  int npages = mb * 256;
  char *base = sbrk(mb * 1024 * 1024);

  if(base == (char*)-1){
    printf("cowtest: sbrk failed\n");
    exit(1);
  }

  // Make all 80 MB physically resident.
  for(int i = 0; i < npages; i++)
    base[i * PAGE] = i % 100;

  int pid = fork();

  if(pid < 0){
    printf("cowtest: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    for(int i = 0; i < npages; i++){
      if(base[i * PAGE] != i % 100){
        printf("cowtest: child sees wrong data\n");
        exit(1);
      }
    }

    for(int i = 0; i < npages; i += 1000)
      base[i * PAGE] = 99;

    exit(0);
  }

  int status;
  if(wait(&status) < 0 || status != 0){
    printf("cowtest: child failed\n");
    exit(1);
  }

  for(int i = 0; i < npages; i++){
    if(base[i * PAGE] != i % 100){
      printf("cowtest: parent data changed\n");
      exit(1);
    }
  }

  printf("cowtest: OK\n");
  exit(0);
}
