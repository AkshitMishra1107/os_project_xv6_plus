#include "kernel/types.h"
#include "kernel/memstat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int writes = argc > 1 && strcmp(argv[1], "writeall") == 0;
  int rounds = argc > 2 ? atoi(argv[2]) : 10;
  if((argc > 1 && !writes && strcmp(argv[1], "readonly") != 0) || rounds < 1 || rounds > 100){
    printf("usage: forkbench readonly|writeall [1..100 rounds]\n");
    exit(1);
  }
  int bytes = 20 * 1024 * 1024;
  char *base = sbrk(bytes);
  if(base == (char *)-1)
    exit(1);
  memset(base, 0x31, bytes);
  struct memstat before, after;
  if(memstat(&before) < 0)
    exit(1);
  uint64 start = uptime_us();
  for(int n = 0; n < rounds; n++){
    int pid = fork(), status;
    if(pid < 0)
      exit(1);
    if(pid == 0){
      if(writes){
        memset(base, 0x5a, bytes);
        for(int i = 0; i < bytes; i++)
          if(base[i] != 0x5a)
            exit(1);
      }
      exit(0);
    }
    if(wait(&status) < 0 || status != 0)
      exit(1);
  }
  uint64 elapsed = uptime_us() - start;
  if(memstat(&after) < 0)
    exit(1);
  for(int i = 0; i < bytes; i++)
    if(base[i] != 0x31){
      printf("forkbench: FAILED parent isolation\n");
      exit(1);
    }
  printf("forkbench: mode=%s rounds=%d MiB=20 elapsed_us=%ld cow_copies=%ld\n",
         writes ? "writeall" : "readonly", rounds, elapsed, after.cow_copies-before.cow_copies);
  printf("forkbench: OK (parent and child data checked)\n");
  exit(0);
}
