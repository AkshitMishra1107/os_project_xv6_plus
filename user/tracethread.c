#include "kernel/types.h"
#include "kernel/syscall.h"
#include "user/user.h"
#include "user/thread.h"

static void worker(void *arg)
{
  int id = thread_self();
  if(id <= 0)
    exit(1);
  // Change this member's mask before it is reaped and its TID reused.
  trace(0);
}

int main(void)
{
  printf("tracethread: inherited mask begins\n");
  trace(1U << SYS_kgettid);
  int tid = thread_create(worker, 0);
  if(tid <= 0 || thread_join(tid) < 0)
    exit(1);
  trace(0);
  printf("tracethread: cleared mask begins\n");
  tid = thread_create(worker, 0);
  if(tid <= 0 || thread_join(tid) < 0)
    exit(1);
  printf("tracethread: OK\n");
  exit(0);
}
