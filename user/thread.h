#ifndef USER_THREAD_H
#define USER_THREAD_H

typedef struct { volatile int state; } mutex_t;
typedef struct { volatile int seq; } cond_t;
typedef struct { mutex_t m; cond_t c; int count; } sem_t;

int thread_create(void (*)(void *), void *);
int thread_join(int);
void thread_exit(void) __attribute__((noreturn));
int thread_self(void);
void mutex_init(mutex_t *);
void mutex_lock(mutex_t *);
void mutex_unlock(mutex_t *);
void cond_init(cond_t *);
void cond_wait(cond_t *, mutex_t *);
void cond_signal(cond_t *);
void cond_broadcast(cond_t *);
void sem_init(sem_t *, int);
void sem_wait(sem_t *);
void sem_post(sem_t *);
#endif
