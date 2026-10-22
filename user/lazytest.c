#include "kernel/types.h"
#include "user/user.h"

#define PAGE 4096

int
main(void)
{
  int mb = 256;
  int npages = mb * 256;
  char *base = sbrk(mb * 1024 * 1024);

  if(base == (char*)-1){
    printf("lazytest: FAIL, sbrk failed\n");
    exit(1);
  }

  int touched = 0;

  for(int i = 0; i < npages; i += 100){
    base[i * PAGE] = 7;
    touched++;
  }

  for(int i = 0; i < npages; i += 100){
    if(base[i * PAGE] != 7){
      printf("lazytest: FAIL, page %d lost data\n", i);
      exit(1);
    }
  }

  // Reading a fresh page must return zero.
  if(base[PAGE] != 0){
    printf("lazytest: FAIL, fresh page is not zero\n");
    exit(1);
  }

  // A syscall must also handle an untouched user buffer.
  char *buf = sbrk(PAGE);
  if(buf == (char*)-1){
    printf("lazytest: FAIL, buffer allocation failed\n");
    exit(1);
  }

  int fd = open("README", 0);
  if(fd < 0){
    printf("lazytest: FAIL, cannot open README\n");
    exit(1);
  }

  int n = read(fd, buf, 100);
  close(fd);

  if(n <= 0){
    printf("lazytest: FAIL, read into lazy buffer failed\n");
    exit(1);
  }

  printf("lazytest: OK (%d pages touched)\n", touched);
  exit(0);
}
