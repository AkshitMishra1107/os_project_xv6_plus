#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"
#include "user/sig.h"
#include "kernel/memstat.h"
#include "user/pagepattern.h"

#define NT          3
#define PAGE        4096
static int per_thread = 2 * 1024 * 1024;

static char *region;
static mutex_t lock;
static int pages_touched;
static int writers_done, bad_pages;
static cond_t initialized;
static volatile int got_signal;

static void on_usr1(int sig);
static void worker(void *arg);

int
main(int argc, char **argv)
{
  int tids[NT];
  struct memstat before, after;
  int parent = getpid();
  int mib = argc > 1 ? atoi(argv[1]) : 2;
  if(mib < 1 || mib > 16)
    exit(1);
  per_thread = mib * 1024 * 1024;

  region = sbrk(NT * per_thread);
  if(region == (char *)-1){
    printf("finaldemo: FAIL reservation\n");
    exit(1);
  }
  mutex_init(&lock);
  cond_init(&initialized);
  signal(SIGUSR1, (uint64)on_usr1);
  memstat(&before);

  int child = fork();
  if(child < 0)
    exit(1);
  if(child == 0){                           // child: signal the parent a little later
    sleep(20);
    sigsend(parent, SIGUSR1);
    exit(0);
  }

  for(int i = 0; i < NT; i++){
    tids[i] = thread_create(worker, (void*)(uint64)i);
    if(tids[i] <= 0){
      printf("finaldemo: FAIL thread creation\n");
      exit(1);
    }
  }
  for(int i = 0; i < NT; i++){
    if(thread_join(tids[i]) < 0){
      printf("finaldemo: FAIL join\n");
      exit(1);
    }
  }

  while(!got_signal)                        // poll: sleep() returns early when a signal arrives
    sleep(1);
  int status;
  if(wait(&status) < 0 || status != 0)
    exit(1);

  memstat(&after);
  printf("finaldemo: %d threads touched %d pages, signal %d received\n",
         NT, pages_touched, got_signal);
  printf("finaldemo: lazy allocations %d, page faults %d\n",
         (int)(after.lazy_allocs - before.lazy_allocs),
         (int)(after.page_faults - before.page_faults));
  printf("finaldemo: MiB=%d full-page bad=%d swap_in=%ld swap_out=%ld\n",
         NT*mib, bad_pages, after.swap_ins-before.swap_ins, after.swap_outs-before.swap_outs);
  exit(pages_touched == NT * (per_thread / PAGE) && got_signal == SIGUSR1 && bad_pages == 0 ? 0 : 1);
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
  char *mine = region + id * per_thread;
  mutex_lock(&lock);
  printf("finaldemo: worker %d running as tid %d\n", id, thread_self());
  mutex_unlock(&lock);

  for(int off = 0; off < per_thread; off += PAGE){
    page_fill(mine + off, (id*per_thread + off)/PAGE, 31);
    mutex_lock(&lock);
    pages_touched++;
    mutex_unlock(&lock);
  }
  mutex_lock(&lock);
  if(++writers_done == NT)
    cond_broadcast(&initialized);
  while(writers_done < NT)
    cond_wait(&initialized, &lock);
  mutex_unlock(&lock);
  for(int off = 0; off < per_thread; off += PAGE)
    if(page_verify(mine + off, (id*per_thread + off)/PAGE, 31) < 0)
      __sync_fetch_and_add(&bad_pages, 1);
}
