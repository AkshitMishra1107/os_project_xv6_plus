#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

#define PAGE 4096
#define NPAGES 128
static char *heap;
static char *growth[4];
static volatile int ready, go;
static volatile int cow_value[1024] __attribute__((aligned(PAGE)));
static volatile int cow_flags[1024] __attribute__((aligned(PAGE)));

static void
touch(void *arg)
{
  int id = (uint64)arg;
  __sync_fetch_and_add(&ready, 1);
  while(!__sync_fetch_and_add(&go, 0))
    futex_wait((int *)&go, 0);
  __sync_fetch_and_add((int *)heap, 1);
  for(int i = id+1; i < NPAGES; i += 4)
    *(int *)(heap+i*PAGE) = i*17;
  growth[id] = sbrk(PAGE);
  CHECK(growth[id] != (char *)-1);
  growth[id][0] = id+1;
}

static void
cow_reader(void *unused)
{
  CHECK(cow_value[0] == 123);
  __sync_lock_test_and_set(&cow_flags[0], 1);
  // No syscall between the first and second read: this exercises cached
  // translations while another member writes an inherited COW page.
  while(!__sync_fetch_and_add(&cow_flags[1], 0))
    ;
  CHECK(cow_value[0] == 456);
}

int
main(void)
{
  char *raw = sbrk((NPAGES+1)*PAGE);
  CHECK(raw != (char *)-1);
  heap = (char *)(((uint64)raw+PAGE-1)&~(uint64)(PAGE-1));
  int tids[4];
  for(int i = 0; i < 4; i++){
    tids[i] = thread_create(touch, (void *)(uint64)i);
    CHECK(tids[i] > 0);
  }
  while(__sync_fetch_and_add(&ready, 0) < 4)
    ;
  __sync_lock_test_and_set(&go, 1);
  futex_wake((int *)&go);
  for(int i = 0; i < 4; i++)
    CHECK(thread_join(tids[i]) == 0);
  CHECK(*(int *)heap == 4);
  for(int i = 1; i < NPAGES; i++)
    CHECK(*(int *)(heap+i*PAGE) == i*17);
  for(int i = 0; i < 4; i++){
    CHECK(growth[i][0] == i+1);
    for(int j = 0; j < i; j++)
      CHECK(growth[i] != growth[j]);
  }
  cow_value[0] = 123;
  int pid = fork(), status;
  CHECK(pid >= 0);
  if(pid == 0){
    int tid = thread_create(cow_reader, 0);
    CHECK(tid > 0);
    while(!__sync_fetch_and_add(&cow_flags[0], 0))
      ;
    cow_value[0] = 456;
    __sync_synchronize();
    __sync_lock_test_and_set(&cow_flags[1], 1);
    CHECK(thread_join(tid) == 0);
    exit(0);
  }
  CHECK(wait(&status) == pid && status == 0 && cow_value[0] == 123);
  printf("threadmem: OK (concurrent lazy faults, shared growth, inherited COW visibility)\n");
  exit(0);
}
