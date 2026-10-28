#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STACK 8192
static char *stacks[MAXTHREADS-1];
static volatile int done[MAXTHREADS-1];
static volatile int gate;
static int parent_pid;
static volatile int errors;
static volatile int value;

static void
end(void)
{
  kthread_exit();
}

static void
worker(void *arg)
{
  int n = (uint64)arg;
  if(kgettid() <= 0 || getpid() != parent_pid)
    __sync_fetch_and_add(&errors, 1);
  while(__sync_fetch_and_add(&gate, 0) == 0)
    ;
  __sync_fetch_and_add(&done[n], 1);
}

static void
fork_worker(void *unused)
{
  value = 77;
  int pid = fork();
  if(pid == 0){
    if(kgettid() != 0 || value != 77)
      exit(2);
    value = 99;
    exit(0);
  }
  int status = -1;
  if(pid < 0 || wait(&status) != pid || status != 0 || value != 77)
    errors++;
  char *argv[] = {"echo", "unexpected-worker-exec", 0};
  if(exec("echo", argv) != -1)
    errors++;
}

static int
create(void (*fn)(void *), int n)
{
  uint64 sp = ((uint64)stacks[n] + STACK) & ~15UL;
  return kthread_create((uint64)fn, n, sp, (uint64)end);
}

int
main(void)
{
  parent_pid = getpid();
  if(kgettid() != 0)
    exit(1);
  for(int n = 0; n < MAXTHREADS-1; n++){
    stacks[n] = malloc(STACK);
    if(stacks[n] == 0)
      exit(1);
  }
  int tids[MAXTHREADS-1];
  for(int n = 0; n < MAXTHREADS-1; n++){
    tids[n] = create(worker, n);
    if(tids[n] < 1){
      printf("kthreadtest: create failed at %d\n", n);
      exit(1);
    }
  }
  if(create(worker, 0) != -1 || kthread_join(0) != -1 ||
     kthread_join(MAXTHREADS) != -1 || sbrk(-4096) != (char *)-1)
    errors++;
  char *argv[] = {"echo", "unexpected-group-exec", 0};
  if(exec("echo", argv) != -1)
    errors++;
  __sync_lock_test_and_set(&gate, 1);
  for(int n = 0; n < MAXTHREADS-1; n++)
    if(kthread_join(tids[n]) != 0 || done[n] != 1)
      errors++;
  if(kthread_join(tids[0]) != -1)
    errors++;
  for(int n = 0; n < 50; n++){
    int tid = create(worker, 0);
    if(tid < 1 || kthread_join(tid) != 0)
      errors++;
  }
  int tid = create(fork_worker, 0);
  if(tid < 1 || kthread_join(tid) != 0)
    errors++;
  uint64 sp = ((uint64)stacks[0] + STACK) & ~15UL;
  if(kthread_create(~0UL, 0, sp, (uint64)end) != -1 ||
     kthread_create((uint64)worker, 0, sp-1, (uint64)end) != -1)
    errors++;
  for(int n = 0; n < MAXTHREADS-1; n++)
    free(stacks[n]);
  printf("kthreadtest: %s (limits, join reuse, PID, threaded fork, exec guards)\n",
         errors ? "FAIL" : "OK");
  exit(errors != 0);
}
