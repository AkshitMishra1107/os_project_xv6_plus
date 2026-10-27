#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/sig.h"
#include "kernel/memstat.h"

#define NT          3
#define PAGE        4096
#define PER_THREAD  (2 * 1024 * 1024)       // each thread touches 2 MB

static char *region;
static mutex_t lock;
static int pages_touched;
static volatile int got_signal;

static void on_usr1(int sig);
static void worker(void *arg);

int
main(void)
{
  int tids[NT];
  struct memstat before, after;
  int parent = getpid();

  region = sbrk(NT * PER_THREAD);           // lazy: nothing is allocated yet
  mutex_init(&lock);
  signal(SIGUSR1, (uint64)on_usr1);
  memstat(&before);

  if(fork() == 0){                          // child: signal the parent a little later
    sleep(20);
    sigsend(parent, SIGUSR1);
    exit(0);
  }

  for(int i = 0; i < NT; i++)
    tids[i] = thread_create(worker, (void*)(uint64)i);
  for(int i = 0; i < NT; i++)
    thread_join(tids[i]);

  while(!got_signal)                        // poll: sleep() returns early when a signal arrives
    sleep(1);
  wait(0);

  memstat(&after);
  printf("finaldemo: %d threads touched %d pages, signal %d received\n",
         NT, pages_touched, got_signal);
  printf("finaldemo: lazy allocations %d, page faults %d\n",
         (int)(after.lazy_allocs - before.lazy_allocs),
         (int)(after.page_faults - before.page_faults));
  exit(pages_touched == NT * (PER_THREAD / PAGE) && got_signal == SIGUSR1 ? 0 : 1);
}

static void
on_usr1(int sig)
{
  got_signal = sig;
}

static void
worker(void *arg)
{
  int id = (int)(uint64)arg;
  char *mine = region + id * PER_THREAD;

  for(int off = 0; off < PER_THREAD; off += PAGE){
    mine[off] = 1;                          // a page fault: lazy allocation
    mutex_lock(&lock);
    pages_touched++;
    mutex_unlock(&lock);
  }
}
