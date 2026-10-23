#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

int
handle_page_fault(struct proc *p, uint64 va, int is_write)
{
  stat_faults++;
  if(va >= p->sz)
    return -1;

  va = PGROUNDDOWN(va);

  pte_t *pte = walk(p->pagetable, va, 0);

  // Case 1: first access to an untouched lazy page.
  if(pte == 0 || (*pte & PTE_V) == 0){
    char *mem = kalloc();

    if(mem == 0)
      return -1;

    memset(mem, 0, PGSIZE);

    if(mappages(p->pagetable, va, PGSIZE, (uint64)mem,
                PTE_R | PTE_W | PTE_U) != 0){
      kfree(mem);
      return -1;
    }
    stat_lazy++;
    return 0;
  }

  // Case 2: write to a copy-on-write page.
  if(is_write && (*pte & PTE_COW)){
    uint64 pa = PTE2PA(*pte);

    if(kref_get((void*)pa) == 1){
      // Already private: restore write permission.
      *pte = (*pte & ~PTE_COW) | PTE_W;
    } else {
      char *mem = kalloc();

      if(mem == 0)
        return -1;

      memmove(mem, (char*)pa, PGSIZE);

      uint64 flags =
        (*pte & (PTE_R | PTE_X | PTE_U)) | PTE_W | PTE_V;

      // Replace this process's mapping with its private copy.
      *pte = PA2PTE(mem) | flags;

      // Release this process's reference to the old page.
      kfree((void*)pa);
      stat_cow++;
    }

    sfence_vma();
    return 0;
  }

  // Invalid access or a write to a genuinely read-only page.
  return -1;
}
