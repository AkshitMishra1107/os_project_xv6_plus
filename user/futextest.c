#include "kernel/types.h"
#include "user/user.h"

static int words[65]; // words[0] and words[64] intentionally hash to one bucket.
static volatile int ack, ready;
static char *stack;

static int
load(volatile int *p)
{
  return __sync_fetch_and_add(p, 0);
}

static void
end(void)
{
  kthread_exit();
}

static void
worker(void *arg)
{
  __sync_lock_test_and_set(&ready, 1);
  for(int n = 1; n <= 200; n++){
    while(load(&words[0]) < n){
      int old = load(&words[0]);
      if(old < n)
        futex_wait(&words[0], old);
    }
    __sync_lock_test_and_set(&ack, n);
    futex_wake((int *)&ack);
  }
}

static int
create(void)
{
  return kthread_create((uint64)worker, 0,
    ((uint64)stack+8192)&~15UL, (uint64)end);
}

int
main(void)
{
  stack = malloc(8192);
  if(stack == 0)
    exit(1);
  if(futex_wait(&words[0], 17) != 0 ||
     futex_wait((int *)((char *)words+1), 0) != -1 ||
     futex_wait((int *)~0UL, 0) != -1 || futex_wake((int *)~0UL) != -1)
    exit(1);
  int tid = create();
  if(tid < 1)
    exit(1);
  while(!load(&ready))
    ;
  sleep(2);
  if(futex_wake(&words[64]) != 0){
    printf("futextest: FAIL hash collision isolation\n");
    exit(1);
  }
  for(int n = 1; n <= 200; n++){
    __sync_lock_test_and_set(&words[0], n);
    futex_wake(&words[0]);
    while(load(&ack) < n){
      int old = load(&ack);
      if(old < n)
        futex_wait((int *)&ack, old);
    }
  }
  if(kthread_join(tid) != 0 || ack != 200)
    exit(1);
  // A child has the same virtual address but a different process key.
  int fds[2];
  if(pipe(fds) < 0)
    exit(1);
  int pid = fork();
  if(pid == 0){
    words[0] = ready = ack = 0;
    close(fds[0]);
    if(create() < 1)
      exit(2);
    while(!load(&ready))
      ;
    sleep(2);
    write(fds[1], "x", 1);
    for(;;)
      sleep(10);
  }
  close(fds[1]);
  char c;
  int status = 0;
  if(pid < 0 || read(fds[0], &c, 1) != 1 || futex_wake(&words[0]) != 0 ||
     kill(pid) != 0 || wait(&status) != pid || status != -1)
    exit(1);
  close(fds[0]);
  free(stack);
  printf("futextest: OK (200 handshakes, mismatch, invalid address, keys, kill)\n");
  exit(0);
}
