# Architecture and implementation map

The project extends the inherited xv6 kernel as one system. User commands call generated syscall stubs; the trampoline selects a member-specific trapframe; the dispatcher routes memory, thread, futex, trace, signal and debugger operations. Page-table updates use ptlock. Group and parent lifetime use wait_lock/member locks. Disk swapping uses a sleep lock and releases spinlocks before I/O.

```mermaid
flowchart TD
 U[Commands and user libraries] --> E[Syscall stubs and member trapframe]
 E --> K[Trap handler and syscall dispatch]
 K --> M[Lazy COW and swapped-page recovery]
 K --> T[Thread lifecycle and futex wait wake]
 K --> O[Tracing signals and child debugger]
 M --> P[Shared page table lock and allocator references]
 M --> S[One-CPU FIFO Clock disk backing]
 T --> P
 O --> P
```

## Supported configurations

| Purpose | CPUs | Managed MiB | Eviction |
|---|---:|---:|---|
| Shared parallel execution and normal regression | 3 | 128 | Disabled |
| Single-CPU normal regression | 1 | 128 | Available under pressure |
| Paging workload | 1 | 8 | FIFO 1 or Clock 2 |

QEMU exposes 128 MiB in all cases. 4 KiB pages and a 16 MiB swap area give 4096 slots. Shared COW frames and kernel pages are not eviction candidates. Single-member fork uses COW; multi-member fork privately copies to avoid unsafe remote cached translations. Exec and shrinking are restricted with multiple members. Debugger targets are single-context direct children.

## Complete file-purpose map

The table compares the eager reference with the integrated source. It lists behavior instead of copying entire C files. Generated build products and inherited unchanged tests are not project additions.

| File | Status | Purpose and main behavior |
|---|---|---|
| `Makefile` | Modified | Registers objects libraries and commands; passes CPU RAM and replacement definitions and appends swap backing to the filesystem image. |
| `kernel/dbg.c` | Added | Implements six direct child control calls with exec breakpoint and exit reasons plus safe abandoned target handling. |
| `kernel/dbg.h` | Added | Defines stop reasons and the PC stack frame return and argument register snapshot exported to the debugger. |
| `kernel/defs.h` | Modified | Declares added memory thread futex signal debugger and statistics helpers so original modules can call them consistently. |
| `kernel/exec.c` | Modified | Rejects unsupported multi member exec and resets caught signal state when installing a new user image. |
| `kernel/futex.c` | Added | Implements 64 keyed buckets expected value wait exact key wake and waiter records shared with proc sleep states. |
| `kernel/kalloc.c` | Modified | Adds kref helpers and free page accounting; kfree releases a physical page only after its last reference. |
| `kernel/main.c` | Modified | Initializes swap metadata page table locking futex buckets and signal state before user processes start. |
| `kernel/memlayout.h` | Modified | Derives PHYSTOP from RAM_MB and reserves separate trapframe virtual addresses with a shared user limit. |
| `kernel/memstat.h` | Added | Defines the seven field structure copied by sys_memstat and read by monitoring and workload programs. |
| `kernel/param.h` | Modified | Defines MAXTHREADS and expands filesystem capacity for the added executable set; does not increase managed RAM. |
| `kernel/pgfault.c` | Added | Created for lazy faults then extended for COW swap permissions and concurrent first touch; handle_page_fault rechecks mappings after allocations. |
| `kernel/pipe.c` | Modified | Stages pipe data so memory recovery and potentially sleeping copies happen outside the pipe spinlock. |
| `kernel/proc.c` | Modified | Adds thread creation join group exit and kill; COW versus private fork selection; trace and signal inheritance; safe reap and debugger abandonment. |
| `kernel/proc.h` | Modified | Adds leader TID member and trapframe metadata plus trace mask pending signals saved handler context and debugger stop state. |
| `kernel/riscv.h` | Modified | Adds COW and swap software flags accessed bit and swap slot encode decode macros used by VM recovery and replacement. |
| `kernel/sigdefs.h` | Added | Defines namespace size supported named signals and default ignore dispositions used by both kernel and user wrappers. |
| `kernel/signal.c` | Added | Implements signal_init pending delivery handler registration send pause and trapframe restoration through sigreturn. |
| `kernel/spinlock.c` | Modified | Exposes interrupt disable nesting through nlocks_held to avoid entering sleepable reclaim from protected contexts; not an exact ownership census. |
| `kernel/swap.c` | Added | Created for memstat then extended with frame slot tracking disk I O FIFO Clock and cleanup counters. |
| `kernel/syscall.c` | Modified | Registers the new handlers and names; uses leader size in argument checks and prints selected returns through a guarded trace mask. |
| `kernel/syscall.h` | Modified | Allocates the added interfaces in numbers 22 through 39 without collisions between subsystems. |
| `kernel/sysfile.c` | Modified | Prefaults I O buffers and stops an opted in debugger target after a successful exec image installation. |
| `kernel/sysproc.c` | Modified | Changes heap growth to shared lazy reservation and exposes memstat threads trace and interruptible sleep handling. |
| `kernel/trampoline.S` | Modified | Replaces the fixed trapframe assumption with sscratch and per member trapframe selection on entry and return. |
| `kernel/trap.c` | Modified | Classifies faults dispatches breakpoint stops delivers signals and returns through the current member trapframe with instruction synchronization. |
| `kernel/vm.c` | Modified | Supports holes COW swap unmapping shared lock protected copies private threaded fork and constrained debugger code writes. |
| `user/baseline.c` | Added | Requests 200 MiB through sbrk and prints success or failure; does not touch the entire reservation. |
| `user/cowtest.c` | Added | Touches 80 MiB forks checks inherited patterns and verifies child writes do not change parent values. |
| `user/dbg.c` | Added | Loads ELF function symbols handles breakpoint patch and software step over and prints registers and bounded frame traces. |
| `user/dbglife.c` | Added | Eight controllers abandon stopped targets; checks completion without recursive wait lock acquisition and rejection of traced threads. |
| `user/dbgmini.c` | Added | Creates an opted in exec child reads its registers and instruction bytes continues it and observes exit reason three. |
| `user/dbgtarget.c` | Added | Uses noinline outer middle and inner functions; final outer five result must be twelve. |
| `user/exittest.c` | Added | Worker calls process exit and external parent verifies whole group termination with status three. |
| `user/finaldemo.c` | Added | Real kernel workers touch three lazy 2 MiB ranges under a mutex and receive a child signal; prints TIDs and checks joins. |
| `user/forkbench.c` | Added | Touches 20 MiB before timing and measures 50 sequential fork child exit wait cycles in guest ticks. |
| `user/futextest.c` | Added | Checks 200 handshakes mismatch invalid addresses exact key isolation and termination behavior. |
| `user/hotcold.c` | Added | Creates a 12 MiB locality pattern for three rounds and prints measured phase swap deltas after initialization. |
| `user/kthreadtest.c` | Added | Checks limits invalid creation identity joins slot reuse threaded fork and exec restrictions. |
| `user/lazytest.c` | Added | Reserves 256 MiB touches every hundredth page checks zero fill and lazy syscall buffers; reports 656 write touched pages. |
| `user/libtest.c` | Added | Checks nested create join mutex increments a condition barrier and semaphore coordination including allocation metadata. |
| `user/memhog.c` | Added | Child writes and checks first and last byte of each requested page; prints mismatch count cumulative swaps and child status. |
| `user/memstat.c` | Added | Prints the seven statistics returned by memstat; it is an observer rather than a pressure workload. |
| `user/philosophers.c` | Added | Five workers acquire forks in a fixed ascending order and each completes 20 meals. |
| `user/prodcons.c` | Added | Two producers and two consumers coordinate capacity eight and verify item count total and multiplicity. |
| `user/racetest.c` | Added | Compares unprotected shared increments with mutex protected increments; protected expected total is 200000. |
| `user/semtest.c` | Added | Six workers perform 60 operations with two permits and verify peak concurrent use does not exceed two. |
| `user/sig.c` | Added | Implements signal registration convenience and the user restorer that invokes sigreturn after a handler returns. |
| `user/sig.h` | Added | Exposes shared signal constants and the convenience wrapper to signal tests and applications. |
| `user/sigtest.c` | Added | Checks self delivery state preservation interprocess pause default termination ignore and interrupted sleep. |
| `user/speedup.c` | Added | Partitions 40 million iterations over selected workers verifies checksums and prints coarse guest ticks. |
| `user/swapfork.c` | Added | Checks first bytes across a fork while child writes second bytes and parent verifies isolation under pressure. |
| `user/thread.c` | Added | Creates and reclaims 16 KiB stacks; implements mutex condition semaphore and TID reuse safe join bookkeeping. |
| `user/thread.h` | Added | Declares real library functions and state types; replaces the development branch sequential placeholder header during integration. |
| `user/threadcheck.h` | Added | Provides common checked assertion reporting used by thread and synchronization tests; no kernel service itself. |
| `user/threadlife.c` | Added | Verifies worker triggered process exit status and whole group kill with parent observation. |
| `user/threadmem.c` | Added | Concurrent lazy first touch shared unique sbrk growth and inherited COW visibility without intervening reader syscalls. |
| `user/threadswap.c` | Added | Four workers write and verify boundary words across 3072 shared pages with a condition barrier. |
| `user/threadtest.c` | Added | Partitions a sum and checks the combined shared result equals 499500. |
| `user/trace.c` | Added | Parses a mask sets tracing and execs a chosen program so selected calls and child activity become visible. |
| `user/tracethread.c` | Added | Checks inherited kgettid tracing with a real member then absence of stale trace events after disabling and reusing slots. |
| `user/user.h` | Modified | Declares memory thread futex tracing signal and debugger calls and required shared structure forward declarations. |
| `user/usys.pl` | Modified | Adds entries that generate assembly ecall wrappers for every implemented user kernel interface. |

`user/usertests.c` is inherited and unchanged against the eager reference. `kernel/pgfault.c` and `kernel/swap.c` were created early and extended later. FIFO and Clock are policy branches inside swap.c. See the final report for the ordered implementation and evidence.
