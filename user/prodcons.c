#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define SIZE 8
#define ITEMS 100
static int buf[SIZE], head, tail, count, total, consumed, seen[ITEMS+1];
static mutex_t lock;
static cond_t not_full, not_empty;

static void
producer(void *unused)
{
  for(int i = 1; i <= ITEMS; i++){
    mutex_lock(&lock);
    while(count == SIZE)
      cond_wait(&not_full, &lock);
    CHECK(count >= 0 && count < SIZE);
    buf[tail] = i;
    tail = (tail+1)%SIZE;
    count++;
    cond_signal(&not_empty);
    mutex_unlock(&lock);
  }
}

static void
consumer(void *unused)
{
  for(int i = 0; i < ITEMS; i++){
    mutex_lock(&lock);
    while(count == 0)
      cond_wait(&not_empty, &lock);
    CHECK(count > 0 && count <= SIZE);
    int value = buf[head];
    head = (head+1)%SIZE;
    count--;
    CHECK(value >= 1 && value <= ITEMS);
    seen[value]++;
    consumed++;
    total += value;
    cond_signal(&not_full);
    mutex_unlock(&lock);
  }
}

int
main(void)
{
  int tids[4];
  mutex_init(&lock);
  cond_init(&not_full);
  cond_init(&not_empty);
  for(int i = 0; i < 4; i++){
    tids[i] = thread_create(i < 2 ? producer : consumer, 0);
    CHECK(tids[i] > 0);
  }
  for(int i = 0; i < 4; i++)
    CHECK(thread_join(tids[i]) == 0);
  CHECK(count == 0 && consumed == 200 && total == 10100);
  for(int i = 1; i <= ITEMS; i++)
    CHECK(seen[i] == 2);
  printf("prodcons: OK total 10100, 200 items, each value consumed twice\n");
  exit(0);
}
