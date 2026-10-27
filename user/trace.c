#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf("usage: trace mask command [args]\n");
    exit(1);
  }
  if(trace(atoi(argv[1])) < 0){
    printf("trace: system call failed\n");
    exit(1);
  }
  exec(argv[2], &argv[2]);
  printf("trace: exec %s failed\n", argv[2]);
  exit(1);
}
