#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"
#include "memstat.h"

#define POLICY_FIFO 1
#define POLICY_CLOCK 2

#ifndef SWAP_POLICY
#define SWAP_POLICY POLICY_FIFO
#endif

#define SWAPSTART FSSIZE
#define SWAPBLOCKS 16384
#define BLOCKS_PER_PAGE (PGSIZE / BSIZE)
#define NSWAPSLOTS (SWAPBLOCKS / BLOCKS_PER_PAGE)

#define NFRAMES ((PHYSTOP - KERNBASE) / PGSIZE)
#define FRAMEIDX(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)
#define FRAMEPA(i) (KERNBASE + (uint64)(i) * PGSIZE)

#define SWAP_LOW_WATER 64

extern struct proc proc[NPROC];

struct frame {
  pagetable_t pt;
  uint64 va;
  uint64 seq;
};

static struct frame frames[NFRAMES];
static uint64 next_seq = 1;

#if SWAP_POLICY == POLICY_CLOCK
static int clock_hand;
#endif

static uint8 slot_used[NSWAPSLOTS];
static struct sleeplock swap_lock;

uint64 stat_faults;
uint64 stat_lazy;
uint64 stat_cow;
uint64 stat_swapin;
uint64 stat_swapout;

void
swap_init(void)
{
 initlock(&ptlock, "ptlock");
  initsleeplock(&swap_lock, "swap");

}

static int
slot_alloc(void)
{
  for(int i = 0; i < NSWAPSLOTS; i++){
    if(slot_used[i] == 0){
      slot_used[i] = 1;
      return i;
    }
  }
  return -1;
}

void
swap_slot_free(int slot)
{
  slot_used[slot] = 0;
}

static int
swap_slots_used(void)
{
  int n = 0;

  for(int i = 0; i < NSWAPSLOTS; i++)
    n += slot_used[i];

  return n;
}

void
memstat_fill(struct memstat *ms)
{
  acquire(&ptlock);

  ms->page_faults = stat_faults;
  ms->lazy_allocs = stat_lazy;
  ms->cow_copies = stat_cow;
  ms->swap_outs = stat_swapout;
  ms->swap_ins = stat_swapin;
  ms->free_pages = kfreepages();
  ms->swap_slots_used = swap_slots_used();

  release(&ptlock);
}

static void
swap_write(int slot, uint64 pa)
{
  for(int k = 0; k < BLOCKS_PER_PAGE; k++){
    struct buf *b =
      bread(ROOTDEV, SWAPSTART + slot * BLOCKS_PER_PAGE + k);

    memmove(b->data, (char *)pa + k * BSIZE, BSIZE);
    bwrite(b);
    brelse(b);
  }
}

static void
swap_read(int slot, uint64 pa)
{
  for(int k = 0; k < BLOCKS_PER_PAGE; k++){
    struct buf *b =
      bread(ROOTDEV, SWAPSTART + slot * BLOCKS_PER_PAGE + k);

    memmove((char *)pa + k * BSIZE, b->data, BSIZE);
    brelse(b);
  }
}

void
swap_track(uint64 pa, pagetable_t pt, uint64 va)
{
  push_off();
  int locked_here = !holding(&ptlock);
  if(locked_here)
    acquire(&ptlock);

  struct frame *f = &frames[FRAMEIDX(pa)];
  f->pt = pt;
  f->va = va;
  f->seq = next_seq++;

  if(locked_here)
    release(&ptlock);
  pop_off();
}

void
swap_untrack(uint64 pa)
{
  push_off();
  int locked_here = !holding(&ptlock);
  if(locked_here)
    acquire(&ptlock);

  frames[FRAMEIDX(pa)].pt = 0;

  if(locked_here)
    release(&ptlock);
  pop_off();
}

void
swap_untrack_owner(uint64 pa, pagetable_t pt)
{
  push_off();
  int locked_here = !holding(&ptlock);
  if(locked_here)
    acquire(&ptlock);

  struct frame *f = &frames[FRAMEIDX(pa)];
  if(f->pt == pt)
    f->pt = 0;

  if(locked_here)
    release(&ptlock);
  pop_off();
}

static int
pt_is_live(pagetable_t pt)
{
  for(struct proc *p = proc; p < &proc[NPROC]; p++){
    if(p->state != UNUSED && p->pagetable == pt)
      return 1;
  }

  return 0;
}

static int
evictable(int i)
{
  struct frame *f = &frames[i];

  if(f->pt == 0 || !pt_is_live(f->pt))
    return 0;

  pte_t *pte = walk(f->pt, f->va, 0);

  if(pte == 0)
    return 0;

  if((*pte & (PTE_V | PTE_U)) != (PTE_V | PTE_U))
    return 0;

  if(*pte & PTE_COW)
    return 0;

  if(PTE2PA(*pte) != FRAMEPA(i))
    return 0;

  if(kref_get((void *)FRAMEPA(i)) != 1)
    return 0;

  return 1;
}

static int
pick_victim(void)
{
#if SWAP_POLICY == POLICY_FIFO
  int best = -1;
  uint64 best_seq = ~(uint64)0;

  for(int i = 0; i < NFRAMES; i++){
    if(frames[i].pt != 0 &&
       frames[i].seq < best_seq &&
       evictable(i)){
      best = i;
      best_seq = frames[i].seq;
    }
  }

  return best;
#else
  for(int n = 0; n < 2 * NFRAMES; n++){
    int i = clock_hand;
    clock_hand = (clock_hand + 1) % NFRAMES;

    if(!evictable(i))
      continue;

    pte_t *pte = walk(frames[i].pt, frames[i].va, 0);

    if(*pte & PTE_A){
      *pte &= ~PTE_A;
      sfence_vma();
      continue;
    }

    return i;
  }

  return -1;
#endif
}

int
swap_out_one(void)
{
  int ret = -1;

  acquiresleep(&swap_lock);
  acquire(&ptlock);

  int i = pick_victim();
  int slot = -1;
  uint64 pa = 0;

  if(i >= 0){
    slot = slot_alloc();

    if(slot >= 0){
      pa = FRAMEPA(i);

      pte_t *pte = walk(frames[i].pt, frames[i].va, 0);
      uint64 perm = *pte & (PTE_R | PTE_W | PTE_X | PTE_U);

      *pte = SLOT2PTE(slot) | PTE_SWAP | perm;
      sfence_vma();
      frames[i].pt = 0;
    }
  }

  release(&ptlock);

  if(i >= 0 && slot >= 0){
    // Disk I/O must run without the page-table spinlock.
    swap_write(slot, pa);

    acquire(&ptlock);
    kfree((void *)pa);
    stat_swapout++;
    release(&ptlock);

    ret = 0;
  }

  releasesleep(&swap_lock);
  return ret;
}

void *
kalloc_user(void)
{
  if(kfreepages() < SWAP_LOW_WATER && nlocks_held() == 0){
    while(kfreepages() < SWAP_LOW_WATER + 16){
      if(swap_out_one() < 0)
        break;
    }
  }

  return kalloc();
}

void
swap_in(pagetable_t pt, uint64 va, pte_t *pte, void *mem)
{
  acquiresleep(&swap_lock);
  acquire(&ptlock);

  // Waiting for the swap lock may have changed the mapping.
  pte = walk(pt, va, 0);
  if(pte == 0 || (*pte & PTE_SWAP) == 0){
    release(&ptlock);
    releasesleep(&swap_lock);
    kfree(mem);
    return;
  }

  pte_t expected = *pte;
  int slot = PTE2SLOT(expected);
  uint64 perm = expected & (PTE_R | PTE_W | PTE_X | PTE_U);

  release(&ptlock);

  swap_read(slot, (uint64)mem);

  acquire(&ptlock);
  pte = walk(pt, va, 0);

  // An unmap during disk I/O must not be undone.
  if(pte && *pte == expected){
    *pte = PA2PTE(mem) | perm | PTE_V;
    swap_slot_free(slot);
    sfence_vma();

    swap_track((uint64)mem, pt, va);
    stat_swapin++;

    // Ownership has transferred to the mapping.
    mem = 0;
  }

  release(&ptlock);
  releasesleep(&swap_lock);

  if(mem)
    kfree(mem);
}
