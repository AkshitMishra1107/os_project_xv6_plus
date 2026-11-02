#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NBUCKET 64
struct bucket { struct spinlock lock; };
static struct bucket buckets[NBUCKET];
extern struct proc proc[NPROC];
// Bucket ownership protects each active record; proc locks protect sleep state.
static struct { struct proc *leader; uint64 addr; int active; } waiters[NPROC];

static struct bucket *
bucket_for(struct proc *leader, uint64 addr)
{
  return &buckets[(((uint64)leader >> 4) ^ (addr >> 2)) % NBUCKET];
}

static int
valid_address(struct proc *p, uint64 addr)
{
  acquire(&ptlock);
  uint64 sz = p->leader->sz;
  int valid = !(addr & (sizeof(int)-1)) && addr < sz && sizeof(int) <= sz-addr;
  release(&ptlock);
  return valid;
}

void
futex_init(void)
{
  for(int i = 0; i < NBUCKET; i++)
    initlock(&buckets[i].lock, "futex");
}

uint64
sys_futex_wait(void)
{
  uint64 addr;
  int expected, value;
  struct proc *p = myproc();
  argaddr(0, &addr);
  argint(1, &expected);
  if(!valid_address(p, addr))
    return -1;
  // A potentially sleeping fault must occur before taking the bucket lock.
  prefault(addr, sizeof(int), 0);
  struct bucket *b = bucket_for(p->leader, addr);
  int slot = p-proc;
  acquire(&b->lock);
  if(copyin(p->pagetable, (char *)&value, addr, sizeof(value)) < 0){
    release(&b->lock);
    return -1;
  }
  if(value == expected && !killed(p)){
    waiters[slot].leader = p->leader;
    waiters[slot].addr = addr;
    waiters[slot].active = 1;
    sleep(b, &b->lock);
    waiters[slot].active = 0;
    waiters[slot].leader = 0;
  }
  release(&b->lock);
  return killed(p) ? -1 : 0;
}

uint64
sys_futex_wake(void)
{
  uint64 addr;
  struct proc *p = myproc();
  argaddr(0, &addr);
  if(!valid_address(p, addr))
    return -1;
  struct bucket *b = bucket_for(p->leader, addr);
  int count = 0;
  acquire(&b->lock);
  for(int i = 0; i < NPROC; i++){
    struct proc *q = &proc[i];
    if(q == p)
      continue;
    acquire(&q->lock);
    if(q->state == SLEEPING && q->chan == b && waiters[i].active &&
       waiters[i].leader == p->leader && waiters[i].addr == addr){
      q->state = RUNNABLE;
      count++;
    }
    release(&q->lock);
  }
  release(&b->lock);
  return count;
}
