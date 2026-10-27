#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

struct spinlock siglock;         // protects the signal state of all processes

void
signal_init(void)
{
  initlock(&siglock, "siglock");
}

// Is a signal waiting for this process to handle?
int
signal_pending(struct proc *p)
{
  return p->sig_pending != 0 && !p->in_handler;
}

// Called from usertrap() just before returning to user mode.
void
deliver_signals(struct proc *p)
{
  if(p->sig_pending == 0 || p->in_handler)
    return;

  acquire(&siglock);
  while(p->sig_pending != 0){
    int sig = 1;
    while((p->sig_pending & (1 << sig)) == 0)
      sig++;
    p->sig_pending &= ~(1 << sig);
    uint64 handler = p->sig_handler[sig];
    uint64 restorer = p->sig_restorer[sig];

    if(handler == SIG_IGN)
      continue;                         // ignored: look at the next one
    release(&siglock);

    if(handler == SIG_DFL)
      exit(-1);                         // default action: terminate the process

    // Run the handler: save the interrupted registers, then make the
    // program "call" handler(sig) that returns into the restorer.
    p->sig_saved = *p->trapframe;
    p->in_handler = 1;
    p->trapframe->epc = handler;
    p->trapframe->a0 = sig;
    p->trapframe->ra = restorer;
    return;
  }
  release(&siglock);
}

// Send signal sig to the process with this pid.
int
sigsend(int pid, int sig)
{
  if(sig <= 0 || sig >= NSIG)
    return -1;
  if(sig == SIGKILL)
    return kill(pid);                   // SIGKILL cannot be caught

  acquire(&siglock);
  for(struct proc *p = proc; p < &proc[NPROC]; p++){
    if(p->pid == pid &&
       p->state != UNUSED && p->state != ZOMBIE){
      p->sig_pending |= (1 << sig);
      wakeup(p);                        // wake a sleeping pause() so it notices
      release(&siglock);
      return 0;
    }
  }
  release(&siglock);
  return -1;
}

uint64
sys_sigaction(void)
{
  int sig;
  uint64 handler, restorer;
  struct proc *l = myproc();

  argint(0, &sig);
  argaddr(1, &handler);
  argaddr(2, &restorer);
  if(sig <= 0 || sig >= NSIG || sig == SIGKILL)
    return -1;
  acquire(&siglock);
  l->sig_handler[sig] = handler;
  l->sig_restorer[sig] = restorer;
  release(&siglock);
  return 0;
}

uint64
sys_sigsend(void)
{
  int pid, sig;

  argint(0, &pid);
  argint(1, &sig);
  return sigsend(pid, sig);
}

// Called by the restorer when a handler returns: put the registers back.
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  if(!p->in_handler)
    return -1;
  *p->trapframe = p->sig_saved;
  p->in_handler = 0;
  return p->trapframe->a0;             // syscall() stores this in a0: it must be the OLD a0
}

// Sleep until a signal arrives. Always returns -1, like Unix pause().
uint64
sys_pause(void)
{
  struct proc *p = myproc();

  acquire(&siglock);
  while(!signal_pending(p) && !killed(p))
    sleep(p, &siglock);                // woken by sigsend
  release(&siglock);
  return -1;
}
