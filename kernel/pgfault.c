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
  pagetable_t pt = p->pagetable;
  char *mem = 0;

  stat_faults++;

  if(va >= p->sz)
    return -1;

  va = PGROUNDDOWN(va);

  for(;;){
    push_off();

    pte_t *pte = walk(pt, va, 0);

    int lazy =
      (pte == 0) || ((*pte & (PTE_V | PTE_SWAP)) == 0);

    int swapped =
      (pte != 0) && (*pte & PTE_SWAP);

    int cow =
      (pte != 0) && (*pte & PTE_V) &&
      is_write && (*pte & PTE_COW);

    int copy =
      cow && kref_get((void *)PTE2PA(*pte)) > 1;

    if((lazy || swapped || copy) && mem == 0){
      // Allocation may sleep while another page is swapped out.
      pop_off();

      if(swapped && nlocks_held() > 0)
        return -1;

      mem = kalloc_user();

      if(mem == 0)
        return -1;

      // Allocation may have slept: inspect the entry again.
      continue;
    }

    // Case 1: restore a page from disk.
    if(swapped){
      pop_off();
      swap_in(pt, va, pte, mem);
      return 0;
    }

    // Case 2: first access to an untouched lazy page.
    if(lazy){
      memset(mem, 0, PGSIZE);

      if(mappages(pt, va, PGSIZE, (uint64)mem,
                  PTE_R | PTE_W | PTE_U) != 0){
        pop_off();
        kfree(mem);
        return -1;
      }

      swap_track((uint64)mem, pt, va);
      stat_lazy++;

      pop_off();
      return 0;
    }

    // Case 3: write to a copy-on-write page.
    if(cow){
      uint64 pa = PTE2PA(*pte);

      if(kref_get((void *)pa) == 1){
        // The page already has only one owner.
        *pte = (*pte & ~PTE_COW) | PTE_W;

        swap_track(pa, pt, va);
        sfence_vma();

        pop_off();

        if(mem)
          kfree(mem);

        return 0;
      }

      memmove(mem, (char *)pa, PGSIZE);

      uint64 flags =
        (*pte & (PTE_R | PTE_X | PTE_U)) | PTE_W | PTE_V;

      *pte = PA2PTE(mem) | flags;

      sfence_vma();
      swap_track((uint64)mem, pt, va);

      kfree((void *)pa);
      stat_cow++;

      pop_off();
      return 0;
    }

    // Invalid access or a genuinely read-only mapping.
    pop_off();

    if(mem)
      kfree(mem);

    return -1;
  }
}
