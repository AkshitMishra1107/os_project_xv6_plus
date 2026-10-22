#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[];

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  uint64 nfree;
} kmem;

#define NPAGES    ((PHYSTOP - KERNBASE) / PGSIZE)
#define PGIDX(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

struct {
  struct spinlock lock;
  int count[NPAGES];
} pgref;

void
kinit(void)
{
  initlock(&kmem.lock, "kmem");
  initlock(&pgref.lock, "pgref");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;

  p = (char*)PGROUNDUP((uint64)pa_start);

  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    // kfree will drop this initial reference to zero.
    pgref.count[PGIDX(p)] = 1;
    kfree(p);
  }
}

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 ||
     (char*)pa < end ||
     (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&pgref.lock);

  if(pgref.count[PGIDX(pa)] < 1)
    panic("kfree: refcount");

  if(--pgref.count[PGIDX(pa)] > 0){
    release(&pgref.lock);
    return;
  }

  release(&pgref.lock);

  // Nobody owns this page now.
  memset(pa, 1, PGSIZE);
  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.nfree++;
  release(&kmem.lock);
}

void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;

  if(r){
    kmem.freelist = r->next;
    kmem.nfree--;
  }

  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE);

    acquire(&pgref.lock);
    pgref.count[PGIDX(r)] = 1;
    release(&pgref.lock);
  }

  return (void*)r;
}

void
kref_inc(void *pa)
{
  acquire(&pgref.lock);
  pgref.count[PGIDX(pa)]++;
  release(&pgref.lock);
}

int
kref_get(void *pa)
{
  int n;

  acquire(&pgref.lock);
  n = pgref.count[PGIDX(pa)];
  release(&pgref.lock);

  return n;
}

uint64
kfreepages(void)
{
  uint64 n;

  acquire(&kmem.lock);
  n = kmem.nfree;
  release(&kmem.lock);

  return n;
}
