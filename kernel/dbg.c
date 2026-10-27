#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "dbg.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

#define DBG_MAXIO 64

// The target must be a child of the caller. Call with wait_lock held.
static struct proc *
find_target(int pid)
{
  struct proc *l = myproc();

  for(struct proc *t = proc; t < &proc[NPROC]; t++){
    if(t->pid == pid && t->parent == l && t->state != UNUSED)
      return t;
  }
  return 0;
}

// A target that is currently stopped, or 0.
static struct proc *
stopped_target(int pid)
{
  acquire(&wait_lock);
  struct proc *t = find_target(pid);
  if(t && (!t->traced || t->dbg_state == 0))
    t = 0;
  release(&wait_lock);
  return t;
}

// Freeze the calling (traced) process and tell its debugger.
// Returns when the debugger calls dbg_cont.
void
dbg_stop(struct proc *p, int reason)
{
  acquire(&wait_lock);
  p->dbg_state = reason;
  wakeup(p->parent);                    // wake the debugger sleeping in dbg_wait
  while(p->dbg_state != 0 && !killed(p))
    sleep(&p->dbg_state, &wait_lock);
  p->dbg_state = 0;
  release(&wait_lock);
}

uint64
sys_dbg_traceme(void)
{
  myproc()->traced = 1;
  return 0;
}

// Sleep until the child stops or exits. Returns the reason, or -1.
uint64
sys_dbg_wait(void)
{
  int pid;
  struct proc *l = myproc();

  argint(0, &pid);
  acquire(&wait_lock);
  for(;;){
    struct proc *t = find_target(pid);
    if(t == 0 || killed(myproc())){
      release(&wait_lock);
      return -1;
    }
    if(t->dbg_state != 0 && t->traced){
      int why = t->dbg_state;
      release(&wait_lock);
      return why;
    }
    acquire(&t->lock);
    int dead = (t->state == ZOMBIE);
    release(&t->lock);
    if(dead){
      release(&wait_lock);
      return DBG_EXITED;
    }
    sleep(l, &wait_lock);               // woken by dbg_stop or by the child's exit
  }
}

uint64
sys_dbg_cont(void)
{
  int pid;

  argint(0, &pid);
  acquire(&wait_lock);
  struct proc *t = find_target(pid);
  if(t == 0 || !t->traced || t->dbg_state == 0){
    release(&wait_lock);
    return -1;
  }
  t->dbg_state = 0;
  wakeup(&t->dbg_state);
  release(&wait_lock);
  return 0;
}

uint64
sys_dbg_getregs(void)
{
  int pid;
  uint64 uregs;
  struct dbgregs r;

  argint(0, &pid);
  argaddr(1, &uregs);
  struct proc *t = stopped_target(pid);
  if(t == 0)
    return -1;
  struct trapframe *tf = t->trapframe;
  r.pc = tf->epc;
  r.ra = tf->ra;
  r.sp = tf->sp;
  r.fp = tf->s0;
  r.a0 = tf->a0;
  r.a1 = tf->a1;
  r.a2 = tf->a2;
  r.a3 = tf->a3;
  r.a4 = tf->a4;
  r.a5 = tf->a5;
  r.a6 = tf->a6;
  r.a7 = tf->a7;
  if(copyout(myproc()->pagetable, uregs, (char*)&r, sizeof(r)) < 0)
    return -1;
  return 0;
}

// dbg_peek(pid, addr, buf, n): copy n bytes from the child's memory.
uint64
sys_dbg_peek(void)
{
  int pid, n;
  uint64 addr, ubuf;
  char kbuf[DBG_MAXIO];

  argint(0, &pid);
  argaddr(1, &addr);
  argaddr(2, &ubuf);
  argint(3, &n);
  if(n <= 0 || n > DBG_MAXIO)
    return -1;
  struct proc *t = stopped_target(pid);
  if(t == 0)
    return -1;
  if(copyin(t->pagetable, kbuf, addr, n) < 0)
    return -1;
  return copyout(myproc()->pagetable, ubuf, kbuf, n) < 0 ? -1 : 0;
}

// dbg_poke(pid, addr, buf, n): copy n bytes into the child's memory, even code.
uint64
sys_dbg_poke(void)
{
  int pid, n;
  uint64 addr, ubuf;
  char kbuf[DBG_MAXIO];

  argint(0, &pid);
  argaddr(1, &addr);
  argaddr(2, &ubuf);
  argint(3, &n);
  if(n <= 0 || n > DBG_MAXIO)
    return -1;
  struct proc *t = stopped_target(pid);
  if(t == 0)
    return -1;
  if(copyin(myproc()->pagetable, kbuf, ubuf, n) < 0)
    return -1;
  return copyout_force(t->pagetable, addr, kbuf, n) < 0 ? -1 : 0;
}
