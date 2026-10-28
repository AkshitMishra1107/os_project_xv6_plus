#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define NT 5
#define MEALS 20
static mutex_t forks[NT];
static int meals[NT];

static void
philosopher(void *arg)
{
  int id = (uint64)arg, next = (id+1)%NT;
  int first = id < next ? id : next;
  int second = id < next ? next : id;
  for(int n = 0; n < MEALS; n++){
    mutex_lock(&forks[first]);
    mutex_lock(&forks[second]);
    meals[id]++;
    sleep(1);
    mutex_unlock(&forks[second]);
    mutex_unlock(&forks[first]);
  }
}

int
main(void)
{
  int tids[NT];
  for(int i = 0; i < NT; i++)
    mutex_init(&forks[i]);
  for(int i = 0; i < NT; i++){
    tids[i] = thread_create(philosopher, (void *)(uint64)i);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < NT; i++){
    CHECK(thread_join(tids[i]) == 0 && meals[i] == MEALS);
    printf("philosopher %d ate %d times\n", i, meals[i]);
  }
  printf("philosophers: OK (fixed fork order)\n");
  exit(0);
}
