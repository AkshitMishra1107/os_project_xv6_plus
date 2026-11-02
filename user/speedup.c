#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

static uint work[7], offset[7], results[7];
static void
spin(void *arg)
{
  int id = (uint64)arg;
  volatile uint value = 0;
  for(uint i = 0; i < work[id]; i++)
    value += offset[id] + i;
  results[id] = value;
}

int
main(int argc, char **argv)
{
  int nt = argc > 1 ? atoi(argv[1]) : 1, tids[7];
  uint total = argc > 2 ? atoi(argv[2]) : 400000000U;
  CHECK(nt >= 1 && nt <= 7 && total >= 1000000U && total <= 1000000000U);
  uint done = 0, sum = 0;
  uint64 start = uptime_us();
  for(int i = 0; i < nt; i++){
    work[i] = total/nt + (i < total%nt);
    offset[i] = done;
    done += work[i];
    tids[i] = thread_create(spin, (void *)(uint64)i);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < nt; i++){
    CHECK(thread_join(tids[i]) == 0);
    uint64 n = work[i];
    CHECK(results[i] == (uint)(n*(n-1)/2 + n*offset[i]));
    sum += results[i];
  }
  uint64 elapsed = uptime_us() - start;
  CHECK(sum == (uint)((uint64)total*(total-1)/2));
  printf("speedup: OK workers=%d iterations=%u elapsed_us=%ld checksum=%u\n",
         nt, total, elapsed, sum);
  exit(0);
}
