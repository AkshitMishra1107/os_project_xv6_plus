#ifndef XV6_MEMSTAT_H
#define XV6_MEMSTAT_H

struct memstat {
  uint64 page_faults;
  uint64 lazy_allocs;
  uint64 cow_copies;
  uint64 swap_outs;
  uint64 swap_ins;
  uint64 free_pages;
  uint64 swap_slots_used;
};

#endif
