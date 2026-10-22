#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// Return 0 when the fault is fixed, or -1 for an invalid access.
int
handle_page_fault(struct proc *p, uint64 va, int is_write)
{
  if(va >= p->sz)
    return -1;

  va = PGROUNDDOWN(va);

  pte_t *pte = walk(p->pagetable, va, 0);

  // An existing mapping faulted because access was not permitted.
  // This also protects the stack guard page.
  if(pte != 0 && (*pte & PTE_V))
    return -1;

  char *mem = kalloc();
  if(mem == 0)
    return -1;

  memset(mem, 0, PGSIZE);

  if(mappages(p->pagetable, va, PGSIZE, (uint64)mem,
              PTE_R | PTE_W | PTE_U) != 0){
    kfree(mem);
    return -1;
  }

  return 0;
}
