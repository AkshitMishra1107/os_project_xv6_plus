#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"

static volatile int made, bad;
static mutex_t lock;
static cond_t changed;
static sem_t slots;
static int ready, go, active, peak, total;

static void
sync_worker(void *unused)
{
  mutex_lock(&lock);
  ready++;
  cond_broadcast(&changed);
  while(!go)
    cond_wait(&changed, &lock);
  mutex_unlock(&lock);
  sem_wait(&slots);
  mutex_lock(&lock);
  if(++active > peak)
    peak = active;
  mutex_unlock(&lock);
  sleep(1);
  mutex_lock(&lock);
  active--;
  mutex_unlock(&lock);
  sem_post(&slots);
  for(int n = 0; n < 1000; n++){
    mutex_lock(&lock);
    total++;
    mutex_unlock(&lock);
  }
}
static void
child(void *unused)
{
  if(thread_self() < 1 || thread_join(thread_self()) != -1)
    __sync_fetch_and_add(&bad, 1);
  __sync_fetch_and_add(&made, 1);
}

static void
maker(void *unused)
{
  for(int n = 0; n < 20; n++){
    int tid = thread_create(child, 0);
    if(tid < 1 || thread_join(tid) != 0)
      __sync_fetch_and_add(&bad, 1);
  }
}

int
main(void)
{
  // Text can legitimately begin at VA zero in xv6.
  if(thread_create((void (*)(void *))~0UL, 0) != -1 ||
     thread_join(-1) != -1 || thread_join(0) != -1)
    exit(1);
  int tids[3];
  for(int i = 0; i < 3; i++){
    tids[i] = thread_create(maker, 0);
    if(tids[i] < 1)
      exit(1);
  }
  for(int i = 0; i < 3; i++)
    if(thread_join(tids[i]) != 0)
      exit(1);
  if(bad || made != 60 || thread_join(tids[0]) != -1){
    printf("libtest: FAIL made=%d bad=%d\n", made, bad);
    exit(1);
  }
  mutex_init(&lock);
  cond_init(&changed);
  sem_init(&slots, 2);
  for(int i = 0; i < 3; i++){
    tids[i] = thread_create(sync_worker, 0);
    if(tids[i] < 1)
      exit(1);
  }
  mutex_lock(&lock);
  while(ready < 3)
    cond_wait(&changed, &lock);
  go = 1;
  cond_broadcast(&changed);
  mutex_unlock(&lock);
  for(int i = 0; i < 3; i++)
    if(thread_join(tids[i]) != 0)
      exit(1);
  if(total != 3000 || peak > 2 || active != 0 || slots.count != 2){
    printf("libtest: FAIL total=%d peak=%d active=%d\n", total, peak, active);
    exit(1);
  }
  printf("libtest: OK (60 nested creates/joins, mutex 3000, condition barrier, semaphore)\n");
  exit(0);
}
