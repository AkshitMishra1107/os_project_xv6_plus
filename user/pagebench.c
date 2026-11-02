#include "kernel/types.h"
#include "kernel/memstat.h"
#include "user/user.h"
#include "user/pagepattern.h"

static uint
next(uint *state)
{
  *state ^= *state << 13;
  *state ^= *state >> 17;
  *state ^= *state << 5;
  return *state;
}

int
main(int argc, char **argv)
{
  char *kind = argc > 1 ? argv[1] : "seq";
  int refs = argc > 2 ? atoi(argv[2]) : 6000;
  uint seed = argc > 3 ? atoi(argv[3]) : 1;
  int mode = strcmp(kind, "seq") == 0 ? 0 :
             strcmp(kind, "uniform") == 0 ? 1 :
             strcmp(kind, "hot") == 0 ? 2 :
             strcmp(kind, "phase") == 0 ? 3 : -1;
  if(mode < 0 || refs < 100 || refs > 100000 || seed == 0){
    printf("usage: pagebench seq|uniform|hot|phase [100..100000 refs] [seed>0]\n");
    exit(1);
  }
  int pages = 3072, bad = 0;
  char *base = sbrk(pages * PATTERN_PAGE);
  if(base == (char *)-1)
    exit(1);
  for(int i = 0; i < pages; i++)
    page_fill(base + i * PATTERN_PAGE, i, seed);
  struct memstat before, after;
  if(memstat(&before) < 0)
    exit(1);
  uint state = seed, sum = 0;
  uint64 start = uptime_us();
  for(int i = 0; i < refs; i++){
    uint r = next(&state);
    int page;
    if(mode == 0)
      page = i % pages;
    else if(mode == 1)
      page = r % pages;
    else {
      int phase = mode == 3 ? (i * 3 / refs) : 0;
      page = r % 100 < 90 ? phase * 1024 + ((r >> 8) % 128)
                          : (r >> 8) % pages;
    }
    uint value = *(volatile uint *)(base + page * PATTERN_PAGE);
    if(value != page_pattern(page, 0, seed))
      bad++;
    sum += value;
  }
  uint64 elapsed = uptime_us() - start;
  if(memstat(&after) < 0)
    exit(1);
  // Full-page verification is outside the measured reference phase.
  for(int i = 0; i < pages; i++)
    if(page_verify(base + i * PATTERN_PAGE, i, seed) < 0)
      bad++;
  printf("pagebench: pattern=%s seed=%u refs=%d MiB=12 elapsed_us=%ld\n",
         kind, seed, refs, elapsed);
  printf("pagebench: faults=%ld swap_in=%ld swap_out=%ld checksum=%u bad=%d\n",
         after.page_faults-before.page_faults,
         after.swap_ins-before.swap_ins, after.swap_outs-before.swap_outs, sum, bad);
  printf("pagebench: %s (all 4096 bytes/page verified)\n", bad ? "FAILED" : "OK");
  exit(bad ? 1 : 0);
}
