#include "kernel/types.h"
#include "user/user.h"
#include "user/sig.h"

static volatile int got;                // set by the handler

void on_usr1(int sig);
void on_usr1_child(int sig);

int
main(void)
{
  int pid, status;

  // 1. a signal sent to ourselves runs the handler, then the program goes on
  signal(SIGUSR1, (uint64)on_usr1);
  int before = 12345;                   // a local variable must survive the handler
  sigsend(getpid(), SIGUSR1);
  if(got != SIGUSR1 || before != 12345){
    printf("sigtest: FAIL 1\n");
    exit(1);
  }
  printf("sigtest: 1 ok, signal to self\n");

  // 2. a signal wakes pause() in another process
  pid = fork();
  if(pid == 0){
    signal(SIGUSR1, (uint64)on_usr1_child);
    pause();
    exit(1);                            // not reached: the handler exits
  }
  sleep(5);
  sigsend(pid, SIGUSR1);
  wait(&status);
  if(status != 0){
    printf("sigtest: FAIL 2\n");
    exit(1);
  }
  printf("sigtest: 2 ok, signal to another process\n");

  // 3. default action: a signal without a handler ends the process
  pid = fork();
  if(pid == 0){
    for(;;)
      ;                                 // spin until killed
  }
  sleep(5);
  sigsend(pid, SIGTERM);
  wait(&status);
  if(status == 0){
    printf("sigtest: FAIL 3\n");
    exit(1);
  }
  printf("sigtest: 3 ok, default action ended the child\n");

  // 4. an ignored signal does nothing
  signal(SIGUSR2, SIG_IGN);
  sigsend(getpid(), SIGUSR2);
  printf("sigtest: 4 ok, ignored signal\n");

  // 5. a signal interrupts sleep(), and the return value survives the handler
  pid = fork();
  if(pid == 0){
    signal(SIGUSR1, (uint64)on_usr1);
    int r = sleep(1000);                // would take about 100 seconds
    exit(r == -1 && got == SIGUSR1 ? 0 : 1);
  }
  sleep(5);
  sigsend(pid, SIGUSR1);
  wait(&status);
  if(status != 0){
    printf("sigtest: FAIL 5\n");
    exit(1);
  }
  printf("sigtest: 5 ok, signal interrupted sleep\n");

  printf("sigtest: ALL OK\n");
  exit(0);
}

void
on_usr1(int sig)
{
  got = sig;
}

void
on_usr1_child(int sig)
{
  printf("child: got signal %d\n", sig);
  exit(0);
}
