#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/threadcheck.h"

static void
worker(void *unused)
{
  sleep(2);
  exit(3);
}

int
main(void)
{
  int pid = fork(), status = 0;
  CHECK(pid >= 0);
  if(pid == 0){
    int tid = thread_create(worker, 0);
    CHECK(tid > 0);
    thread_join(tid);
    printf("exittest: FAIL main survived worker process exit\n");
    exit(1);
  }
  CHECK(wait(&status) == pid && status == 3);
  printf("exittest: OK (worker exit terminated group, parent received status 3)\n");
  exit(0);
}
