#ifndef XV6_SWAPAUDIT_H
#define XV6_SWAPAUDIT_H
#define SWAPAUDIT_MAX_OWNERS 64
#define SWAPAUDIT_ENTRIES 16
struct swapentry {
  int slot, pid;
  uint64 va;
};
struct swapowner {
  int pid, tid, state, slots;
  uint64 example_va;
  char name[16];
};
struct swapaudit {
  uint64 used, references, orphans, missing, duplicates, invalid;
  uint64 page_tables, owners;
  struct swapowner owner[SWAPAUDIT_MAX_OWNERS];
  uint64 entries;
  struct swapentry entry[SWAPAUDIT_ENTRIES];
};
#endif
