#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

static sem_t slots;
static mutex_t lock;
static int active, peak, completed;

static void
worker(void *unused)
{
  for(int i = 0; i < 10; i++){
    sem_wait(&slots);
    mutex_lock(&lock);
    if(++active > peak)
      peak = active;
    CHECK(active <= 2);
    mutex_unlock(&lock);
    sleep(1);
    mutex_lock(&lock);
    active--;
    completed++;
    mutex_unlock(&lock);
    sem_post(&slots);
  }
}

int
main(void)
{
  int tids[6];
  sem_init(&slots, 2);
  mutex_init(&lock);
  for(int i = 0; i < 6; i++){
    tids[i] = thread_create(worker, 0);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < 6; i++)
    CHECK(thread_join(tids[i]) == 0);
  CHECK(active == 0 && completed == 60 && peak <= 2 && slots.count == 2);
  printf("semtest: OK completed=%d peak=%d permits=2\n", completed, peak);
  exit(0);
}
