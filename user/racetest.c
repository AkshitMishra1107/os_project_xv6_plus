#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define NT 4
#define ITERS 50000
static volatile int ready, go, counter;
static mutex_t lock;

static void
worker(void *arg)
{
  __sync_fetch_and_add(&ready, 1);
  while(!__sync_fetch_and_add(&go, 0))
    futex_wait((int *)&go, 0);
  for(int i = 0; i < ITERS; i++){
    if(arg)
      mutex_lock(&lock);
    // Deliberately separate volatile load/store for the race demonstration.
    int old = counter;
    counter = old+1;
    if(arg)
      mutex_unlock(&lock);
  }
}

static int
run(int safe)
{
  int tids[NT];
  ready = go = counter = 0;
  for(int i = 0; i < NT; i++){
    tids[i] = thread_create(worker, (void *)(uint64)safe);
    CHECK(tids[i] > 0);
  }
  while(__sync_fetch_and_add(&ready, 0) < NT)
    ;
  __sync_lock_test_and_set(&go, 1);
  futex_wake((int *)&go);
  for(int i = 0; i < NT; i++)
    CHECK(thread_join(tids[i]) == 0);
  return counter;
}

int
main(void)
{
  mutex_init(&lock);
  int unsafe = run(0), safe = run(1);
  printf("without lock: expected %d, got %d\n", NT*ITERS, unsafe);
  printf("with mutex:   expected %d, got %d\n", NT*ITERS, safe);
  CHECK(safe == NT*ITERS);
  printf("racetest: OK (unprotected result depends on scheduling)\n");
  exit(0);
}
