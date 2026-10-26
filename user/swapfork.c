#include "kernel/types.h"
#include "user/user.h"

#define PAGE 4096

int
main(void)
{
  int mb = 6;
  int npages = mb * 256;

  char *base = sbrk(mb * 1024 * 1024);

  if(base == (char *)-1){
    printf("swapfork: sbrk failed\n");
    exit(1);
  }

  // Fill the parent's pages before sharing them.
  for(int i = 0; i < npages; i++)
    base[i * PAGE] = (char)(i % 251);

  int pid = fork();

  if(pid < 0){
    printf("swapfork: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    int bad = 0;

    // Check the inherited data.
    for(int i = 0; i < npages; i++){
      if(base[i * PAGE] != (char)(i % 251))
        bad++;
    }

    // Private writes force COW copies under memory pressure.
    for(int i = 0; i < npages; i++)
      base[i * PAGE + 1] = 77;

    // Check both original data and private changes.
    for(int i = 0; i < npages; i++){
      if(base[i * PAGE] != (char)(i % 251) ||
         base[i * PAGE + 1] != 77)
        bad++;
    }

    printf("swapfork: child bad=%d\n", bad);
    exit(bad ? 1 : 0);
  }

  int status = -2;

  if(wait(&status) < 0){
    printf("swapfork: wait failed\n");
    exit(1);
  }

  int bad = (status != 0);

  if(status != 0)
    printf("swapfork: child exit status %d\n", status);

  // The parent's data must remain unchanged.
  for(int i = 0; i < npages; i++){
    if(base[i * PAGE] != (char)(i % 251) ||
       base[i * PAGE + 1] != 0)
      bad++;
  }

  printf("swapfork: %s\n", bad ? "FAIL" : "OK");
  exit(bad ? 1 : 0);
}
