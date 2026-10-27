#ifndef USER_THREAD_H
#define USER_THREAD_H

#include "kernel/types.h"

typedef struct {
  uint locked;
} mutex_t;

static inline void mutex_init(mutex_t *m) { m->locked = 0; }
static inline void mutex_lock(mutex_t *m) { (void)m; }
static inline void mutex_unlock(mutex_t *m) { (void)m; }

static inline int thread_create(void (*fn)(void*), void *arg) {
  fn(arg);
  return 0;
}
static inline int thread_join(int tid) { (void)tid; return 0; }

#endif
