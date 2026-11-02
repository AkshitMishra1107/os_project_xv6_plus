#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define TOTAL 40000000U
static uint work[7], results[7];
static void
spin(void *arg)
{
  int id = (uint64)arg;
  volatile uint value = 0;
  for(uint i = 0; i < work[id]; i++)
    value += i;
  results[id] = value;
}

int
main(int argc, char **argv)
{
  int nt = argc == 2 ? atoi(argv[1]) : 1, tids[7];
  CHECK(nt >= 1 && nt <= 7);
  int start = uptime();
  for(int i = 0; i < nt; i++){
    work[i] = TOTAL/nt + (i < TOTAL%nt);
    tids[i] = thread_create(spin, (void *)(uint64)i);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < nt; i++){
    CHECK(thread_join(tids[i]) == 0);
    uint64 n = work[i];
    CHECK(results[i] == (uint)(n*(n-1)/2));
  }
  printf("speedup: OK %d thread(s), 40000000 iterations, %d ticks\n", nt, uptime()-start);
  exit(0);
}
