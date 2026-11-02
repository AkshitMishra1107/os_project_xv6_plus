#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// Called without ptlock held; returns without holding it.
int
handle_page_fault(struct proc *p, uint64 va, int access)
{
  pagetable_t pt = p->pagetable;
  uint64 pageva = PGROUNDDOWN(va);
  int is_write = (access == ACC_WRITE);
  char *mem = 0;

  acquire(&ptlock);
  stat_faults++;

  for(;;){
    // Recheck the shared size after every possible allocation.
    if(va >= p->leader->sz || va >= USERLIMIT)
      goto bad;

    pte_t *pte = walk(pt, pageva, 0);

    int lazy =
      (pte == 0) || ((*pte & (PTE_V | PTE_SWAP)) == 0);

    int swapped =
      (pte != 0) && (*pte & PTE_SWAP);

    int cow =
      (pte != 0) && (*pte & PTE_V) && (*pte & PTE_U) &&
      is_write && (*pte & PTE_COW);

    int copy =
      cow && kref_get((void *)PTE2PA(*pte)) > 1;

    // Lazy heap pages are not executable; guard pages are not user pages.
    if((lazy && access == ACC_EXEC) ||
       (!lazy && (*pte & PTE_U) == 0))
      goto bad;

    if((lazy || swapped || copy) && mem == 0){
      // Allocation may sleep while swapping another page out.
      release(&ptlock);

      if(swapped && nlocks_held() > 0)
        return -1;

      mem = kalloc_user();
      if(mem == 0)
        return -1;

      acquire(&ptlock);
      continue;
    }

    // Restore a swapped page without holding a spinlock.
    if(swapped){
      release(&ptlock);
      swap_in(pt, pageva, pte, mem);
      return 0;
    }

    // First access to an untouched lazy page.
    if(lazy){
      memset(mem, 0, PGSIZE);

      if(mappages(pt, pageva, PGSIZE, (uint64)mem,
                  PTE_R | PTE_W | PTE_U) != 0)
        goto bad;

      swap_track((uint64)mem, pt, pageva);
      stat_lazy++;

      release(&ptlock);
      return 0;
    }

    // Write to a copy-on-write page.
    if(cow){
      uint64 pa = PTE2PA(*pte);

      if(kref_get((void *)pa) == 1){
        *pte = (*pte & ~PTE_COW) | PTE_W;
        swap_track(pa, pt, pageva);
        sfence_vma();

        release(&ptlock);
        if(mem)
          kfree(mem);
        return 0;
      }

      memmove(mem, (char *)pa, PGSIZE);

      uint64 flags =
        (*pte & (PTE_R | PTE_X | PTE_U)) | PTE_W | PTE_V;

      *pte = PA2PTE(mem) | flags;

      sfence_vma();
      swap_track((uint64)mem, pt, pageva);
      kfree((void *)pa);
      stat_cow++;

      release(&ptlock);
      return 0;
    }

    // Another thread may already have repaired the mapping.
    uint64 need = PTE_V | PTE_U |
      (access == ACC_WRITE ? PTE_W :
       access == ACC_EXEC ? PTE_X : PTE_R);

    int ok = pte && ((*pte & need) == need);

    release(&ptlock);
    if(mem)
      kfree(mem);
    return ok ? 0 : -1;
  }

bad:
  release(&ptlock);
  if(mem)
    kfree(mem);
  return -1;
}
