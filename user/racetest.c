#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define NT 4
#define ROUNDS 1000
static volatile int counter;
static mutex_t lock, barrier_lock;
static cond_t barrier_changed;
static int arrived, generation;

static void
barrier(void)
{
  mutex_lock(&barrier_lock);
  int old = generation;
  if(++arrived == NT){
    arrived = 0;
    generation++;
    cond_broadcast(&barrier_changed);
  } else {
    while(old == generation)
      cond_wait(&barrier_changed, &barrier_lock);
  }
  mutex_unlock(&barrier_lock);
}

static void
worker(void *arg)
{
  int safe = (uint64)arg;
  for(int i = 0; i < ROUNDS; i++){
    barrier();
    if(safe){
      mutex_lock(&lock);
      counter++;
      mutex_unlock(&lock);
    } else {
      int old = counter;
      // Force all readers to take the same old value before any store.
      barrier();
      counter = old + 1;
    }
    barrier();
  }
}

static int
run(int safe)
{
  int tids[NT];
  counter = arrived = generation = 0;
  for(int i = 0; i < NT; i++){
    tids[i] = thread_create(worker, (void *)(uint64)safe);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < NT; i++)
    CHECK(thread_join(tids[i]) == 0);
  return counter;
}

int
main(void)
{
  mutex_init(&lock);
  mutex_init(&barrier_lock);
  cond_init(&barrier_changed);
  int unsafe = run(0), safe = run(1);
  printf("racetest: forced read-before-write interleaving, %d workers, %d rounds\n", NT, ROUNDS);
  printf("without lock: expected %d, got %d, lost %d\n", NT*ROUNDS, unsafe, NT*ROUNDS-unsafe);
  printf("with mutex: expected %d, got %d\n", NT*ROUNDS, safe);
  CHECK(unsafe == ROUNDS && safe == NT*ROUNDS);
  printf("racetest: OK (controlled lost update and protected invariant)\n");
  exit(0);
}
