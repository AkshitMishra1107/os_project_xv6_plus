#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#include "kernel/memstat.h"
#define PAGE 4096
#define NPAGES 3072
static char *heap;
static mutex_t lock;
static cond_t changed;
static int arrived;

static void
worker(void *arg)
{
  int id = (uint64)arg;
  for(int i = id; i < NPAGES; i += 4){
    *(uint *)(heap+i*PAGE) = i*17+1;
    *(uint *)(heap+(i+1)*PAGE-4) = ~((uint)i*17+1);
  }
  mutex_lock(&lock);
  arrived++;
  cond_broadcast(&changed);
  while(arrived < 4)
    cond_wait(&changed, &lock);
  mutex_unlock(&lock);
  for(int i = id; i < NPAGES; i += 4){
    CHECK(*(uint *)(heap+i*PAGE) == i*17+1);
    CHECK(*(uint *)(heap+(i+1)*PAGE-4) == ~((uint)i*17+1));
  }
}

int
main(void)
{
  struct memstat before, after;
  CHECK(memstat(&before) == 0);
  char *raw = sbrk((NPAGES+1)*PAGE);
  CHECK(raw != (char *)-1);
  heap = (char *)(((uint64)raw+PAGE-1)&~(uint64)(PAGE-1));
  mutex_init(&lock);
  cond_init(&changed);
  int tids[4];
  for(int i = 0; i < 4; i++){
    tids[i] = thread_create(worker, (void *)(uint64)i);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < 4; i++)
    CHECK(thread_join(tids[i]) == 0);
  CHECK(memstat(&after) == 0);
  printf("threadswap: OK (4 workers, 12 MB, 3072 pages, 0 bad)\n");
  printf("threadswap: swap outs %lu, swap ins %lu (workload deltas)\n",
         after.swap_outs-before.swap_outs, after.swap_ins-before.swap_ins);
  exit(0);
}
