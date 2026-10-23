#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "memstat.h"

uint64 stat_faults, stat_lazy, stat_cow;
uint64 stat_swapin, stat_swapout;

void
memstat_fill(struct memstat *ms)
{
  ms->page_faults = stat_faults;
  ms->lazy_allocs = stat_lazy;
  ms->cow_copies = stat_cow;
  ms->swap_outs = stat_swapout;
  ms->swap_ins = stat_swapin;
  ms->free_pages = kfreepages();
  ms->swap_slots_used = 0;
}
