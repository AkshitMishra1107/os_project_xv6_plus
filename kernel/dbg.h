#ifndef KERNEL_DBG_H
#define KERNEL_DBG_H

#include "types.h"

// Reasons returned by dbg_wait.
#define DBG_EXEC   1     // stopped right after exec, before the first instruction
#define DBG_BREAK  2     // stopped at a breakpoint
#define DBG_EXITED 3     // the program has exited

struct dbgregs {
  uint64 pc, ra, sp, fp;
  uint64 a0, a1, a2, a3, a4, a5, a6, a7;
};

#endif
