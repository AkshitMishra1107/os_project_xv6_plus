#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "user/thread.h"

#define STACKSIZE 16384
static char *stacks[MAXTHREADS];
static char *joining[MAXTHREADS];
// Serializes only library-owned allocation and stack metadata. Applications
// must still protect their own calls to xv6 malloc/free.
static volatile int stack_guard;

static void
guard_lock(void)
{
  while(__sync_lock_test_and_set(&stack_guard, 1))
    ;
  __sync_synchronize();
}

static void
guard_unlock(void)
{
  __sync_synchronize();
  __sync_lock_release(&stack_guard);
}

static void
thread_start_stub(void)
{
  kthread_exit();
}

int
thread_create(void (*fn)(void *), void *arg)
{
  guard_lock();
  char *stk = malloc(STACKSIZE);
  if(stk == 0){
    guard_unlock();
    return -1;
  }
  uint64 top = ((uint64)stk+STACKSIZE)&~15UL;
  int tid = kthread_create((uint64)fn, (uint64)arg, top,
                          (uint64)thread_start_stub);
  if(tid < 0){
    free(stk);
  } else {
    stacks[tid] = stk;
    joining[tid] = 0;
  }
  guard_unlock();
  return tid;
}

int
thread_join(int tid)
{
  if(tid <= 0 || tid >= MAXTHREADS || tid == thread_self())
    return -1;
  guard_lock();
  char *stk = stacks[tid];
  if(stk == 0 || joining[tid] == stk){
    guard_unlock();
    return -1;
  }
  joining[tid] = stk;
  guard_unlock();
  // Do not hold the allocation guard while waiting: the worker may create
  // or join another thread before it finishes.
  int result = kthread_join(tid);
  guard_lock();
  if(result == 0){
    free(stk);
    // The kernel slot can already have been reused by another creator.
    if(stacks[tid] == stk)
      stacks[tid] = 0;
  }
  if(joining[tid] == stk)
    joining[tid] = 0;
  guard_unlock();
  return result;
}

void
thread_exit(void)
{
  kthread_exit();
}

int
thread_self(void)
{
  return kgettid();
}

void
mutex_init(mutex_t *m)
{
  m->state = 0;
}

void
mutex_lock(mutex_t *m)
{
  if(__sync_val_compare_and_swap(&m->state, 0, 1) == 0)
    return;
  while(__sync_lock_test_and_set(&m->state, 2) != 0)
    futex_wait((int *)&m->state, 2);
}

void
mutex_unlock(mutex_t *m)
{
  __sync_synchronize();
  if(__sync_lock_test_and_set(&m->state, 0) == 2)
    futex_wake((int *)&m->state);
}

void
cond_init(cond_t *c)
{
  c->seq = 0;
}

void
cond_wait(cond_t *c, mutex_t *m)
{
  int seq = __sync_fetch_and_add(&c->seq, 0);
  mutex_unlock(m);
  futex_wait((int *)&c->seq, seq);
  mutex_lock(m);
}

void
cond_signal(cond_t *c)
{
  __sync_fetch_and_add(&c->seq, 1);
  futex_wake((int *)&c->seq);
}

void
cond_broadcast(cond_t *c)
{
  cond_signal(c);
}

void
sem_init(sem_t *s, int value)
{
  mutex_init(&s->m);
  cond_init(&s->c);
  s->count = value;
}

void
sem_wait(sem_t *s)
{
  mutex_lock(&s->m);
  while(s->count == 0)
    cond_wait(&s->c, &s->m);
  s->count--;
  mutex_unlock(&s->m);
}

void
sem_post(sem_t *s)
{
  mutex_lock(&s->m);
  s->count++;
  cond_signal(&s->c);
  mutex_unlock(&s->m);
}
