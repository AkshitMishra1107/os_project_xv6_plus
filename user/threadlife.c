#include "kernel/types.h"
#include "user/user.h"

#define STACK 8192
static char *stack;

static void
end(void)
{
  kthread_exit();
}

static void
exit_worker(void *arg)
{
  sleep(2);
  exit(3);
}

static void
sleep_worker(void *arg)
{
  for(;;)
    sleep(10);
}

static int
create(void (*fn)(void *))
{
  uint64 sp = ((uint64)stack + STACK) & ~15UL;
  return kthread_create((uint64)fn, 0, sp, (uint64)end);
}

int
main(void)
{
  stack = malloc(STACK);
  if(stack == 0)
    exit(1);
  int pid = fork();
  if(pid == 0){
    int tid = create(exit_worker);
    if(tid < 1)
      exit(7);
    kthread_join(tid);
    // exit(3) from a worker must terminate the leader before returning here.
    printf("threadlife: FAIL group exit returned\n");
    exit(8);
  }
  int status = -2;
  if(pid < 0 || wait(&status) != pid || status != 3){
    printf("threadlife: FAIL group exit status %d\n", status);
    exit(1);
  }
  int pipefd[2];
  if(pipe(pipefd) < 0)
    exit(1);
  pid = fork();
  if(pid == 0){
    close(pipefd[0]);
    if(create(sleep_worker) < 1)
      exit(9);
    write(pipefd[1], "x", 1);
    for(;;)
      sleep(10);
  }
  close(pipefd[1]);
  char c;
  if(pid < 0 || read(pipefd[0], &c, 1) != 1 || kill(pid) != 0 ||
     wait(&status) != pid || status != -1){
    printf("threadlife: FAIL group kill status %d\n", status);
    exit(1);
  }
  close(pipefd[0]);
  free(stack);
  printf("threadlife: OK (worker exit status 3 and whole-group kill)\n");
  exit(0);
}
