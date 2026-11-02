# Memory extensions

The [current validation](../results/validation-upgrade/README.md) covers the integrated source. [Historical memory results](../results/memory/README.md) retain the earlier checkpoints.

## Deferred heap backing

The eager reference's `sys_sbrk` calls `growproc` to allocate positive heap growth immediately. The project validates and reserves the leader's virtual range first. `handle_page_fault` in the created `kernel/pgfault.c` zeroes and maps a 4096-byte frame on valid first use. `kernel/vm.c` tolerates lazy holes and recovers syscall user buffers. An invalid access retains its protection failure.

`baseline.c` requests 200 MiB without touching all of it. `lazytest.c` reserves 256 MiB, explicitly writes 656 pages, and checks zero fill and lazy buffers. This measures deferred commitment. The initially cloned newer upstream already offered `sbrklazy`; its ordinary `sbrk` remained eager. The historical reference and API are identified in the version table.

## Copy-on-write fork

Original `uvmcopy` copies resident frames. The modified path shares eligible frames, clears write permission, sets `PTE_COW` and increments references. `kernel/kalloc.c` frees a frame at its final release. A write either copies shared backing or restores write permission for its final owner. `copyout` follows the same isolation rule.

`cowtest.c` checks an 80 MiB parent, inherited values, private child writes and unchanged parent data. Revised `forkbench.c` compares ten read-only or write-all children against the same 20 MiB parent. `FORK_COW=0/1` changes only eager/private versus COW fork selection in the same source. Full child and parent data checks accompany microsecond timing. Multithreaded parents use private copying for translation safety.

## Paging and victim selection

`RAM_MB=8` limits xv6's managed physical range, including kernel overhead. QEMU still exposes 128 MiB. `kernel/swap.c` tracks eligible frames and 4096 slots in a fixed 16 MiB disk area. Each 4 KiB page uses four 1024-byte blocks. Eviction replaces a resident PTE with a slot record; a later fault restores previous contents. Disk I/O runs outside `ptlock`.

- Policy 1, FIFO: oldest eligible insertion sequence.
- Policy 2, Clock: circular hand, accessed-bit clearing and second chances.
- Policy 3, seeded Random: bounded random frame probes with a scan fallback.

Shared COW, kernel and stale frame records are ineligible. Eviction requires one CPU; SMP builds run parallel workers with eviction disabled until remote TLB shootdown is implemented.

`memhog.c` fills and verifies **all 4096 bytes per page** through the position-dependent pattern in `pagepattern.h`. `swapfork.c` checks inherited values and private writes under pressure. `threadswap.c` checks shared swapped mappings. `finaldemo 4` combines three real workers, 12 MiB of fully checked data, a barrier, protected totals and a signal while paging runs.

`pagebench.c` compares sequential, uniform, stable-hot and moving-hot reference streams with identical seeded inputs for all policies. It records recovery calls, swap operations, elapsed microseconds and a checked checksum. Full-page verification occurs after the timed phase. The independent runner also replays FIFO, Clock, LRU and optimal models at a separately labelled fixed frame budget.

## Counters and cleanup audit

`memstat.h`, `sys_memstat`, `memstat_fill` and `user/memstat.c` expose recovery calls, lazy allocations, actual COW copies, swap outs, swap ins, free pages and slots used. The first five accumulate; the last two describe current resources.

`swapcheck.c` and `swapaudit.h` add a complete one-CPU bitmap/PTE reconciliation. Under the swap lock and `ptlock`, the audit deduplicates shared page tables and reports owners, virtual addresses, orphan slots, missing backing, duplicate references and invalid records. Recorded remaining slots belong to live sleeping `init` mappings; the checks report zero reconciliation errors. Swap-in and unmap/reaping release slots when their data is no longer needed.

See [tools/README.md](../../tools/README.md) for reproducible full regression, integrity, policy, fork and scaling experiments, with raw logs and native QEMU captures.
