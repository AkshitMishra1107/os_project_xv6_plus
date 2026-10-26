#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#define PIPESIZE 512

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;     // number of bytes read
  uint nwrite;    // number of bytes written
  int readopen;   // read fd is still open
  int writeopen;  // write fd is still open
};

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  if((pi = (struct pipe*)kalloc()) == 0)
    goto bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  return 0;

 bad:
  if(pi)
    kfree((char*)pi);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}

void
pipeclose(struct pipe *pi, int writable)
{
  acquire(&pi->lock);
  if(writable){
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } else {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  if(pi->readopen == 0 && pi->writeopen == 0){
    release(&pi->lock);
    kfree((char*)pi);
  } else
    release(&pi->lock);
}

int
pipewrite(struct pipe *pi, uint64 addr, int n)
{
  int i = 0;
  struct proc *pr = myproc();
  char buf[PIPESIZE];

  while(i < n){
    acquire(&pi->lock);

    if(pi->readopen == 0 || killed(pr)){
      release(&pi->lock);
      return -1;
    }

    release(&pi->lock);

    // Stage user data without holding the pipe lock.
    int want = n - i;
    if(want > PIPESIZE)
      want = PIPESIZE;

    int got = 0;

    while(got < want){
      if(copyin(pr->pagetable, &buf[got],
                addr + i + got, 1) < 0)
        break;
      got++;
    }

    if(got == 0)
      return i;

    acquire(&pi->lock);

    int j = 0;

    while(j < got){
      if(pi->readopen == 0 || killed(pr)){
        release(&pi->lock);
        return -1;
      }

      if(pi->nwrite == pi->nread + PIPESIZE){
        wakeup(&pi->nread);
        sleep(&pi->nwrite, &pi->lock);
      } else {
        pi->data[pi->nwrite++ % PIPESIZE] = buf[j++];
        i++;
      }
    }

    wakeup(&pi->nread);
    release(&pi->lock);

    if(got < want)
      break;
  }

  return i;
}

int
piperead(struct pipe *pi, uint64 addr, int n)
{
  int i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);

  while(pi->nread == pi->nwrite && pi->writeopen){
    if(killed(pr)){
      release(&pi->lock);
      return -1;
    }

    sleep(&pi->nread, &pi->lock);
  }

  while(i < n && pi->nread != pi->nwrite){
    // Stage one byte before releasing the pipe lock.
    char ch = pi->data[pi->nread++ % PIPESIZE];

    wakeup(&pi->nwrite);
    release(&pi->lock);

    // This user copy may now allocate or swap in a page.
    if(copyout(pr->pagetable, addr + i, &ch, 1) < 0)
      return i;

    i++;
    acquire(&pi->lock);
  }

  wakeup(&pi->nwrite);
  release(&pi->lock);

  return i;
}
