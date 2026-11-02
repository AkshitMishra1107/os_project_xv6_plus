# Presenting the memory work to a teacher

Use the [self-contained implementation walkthrough](memory-implementation-walkthrough.docx) to explain the memory mechanisms, code changes and observed comparisons. The [detailed report](../reports/memory-management-draft.docx), especially sections 17–19, supplies the longer demonstration and evidence checklist. Keep the [reference map](../references.md) open for the MIT assignment correspondence and primary sources.

## Demonstration order

| Step | Show or run | Explain |
|---|---|---|
| 1 | README, v0.0/v0.4 tags and references | MIT xv6 is the base; these are educational implementations and integration results. |
| 2 | [Eager result](../results/memory/outputs/eager-baseline.txt), then baseline and lazytest | Large virtual reservation succeeds lazily; 656 sparse-write touches are not total process RSS. |
| 3 | cowtest and forkbench | Show child/parent isolation and distinguish live ticks from historical single observations. |
| 4 | memstat before and after | Counters are cumulative and include shell/measurement activity. |
| 5 | memhog 4 and memhog 12 in the 8 MiB build | The control and pressure workloads verify bytes; reserve success alone is insufficient. |
| 6 | swapfork and the completed regression evidence | Show mechanisms working together and existing tests still passing. |
| 7 | FIFO and Clock measured-phase captures | Compare operations, not an unmeasured claim of elapsed-time speedup. |

## Normal build

```bash
make clean
make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1
```

Run individually at the xv6 prompt:

```text
memstat
baseline
lazytest
cowtest
forkbench
memstat
```

Exit using Ctrl+A then X. For pressure, rebuild clean with `CPUS=1 RAM_MB=8 SWAP_POLICY=1` and run `memhog 4`, `memhog 12`, `swapfork`, `memstat`. Repeat a fresh build with policy 2 for Clock. QEMU exposes 128 MiB physically; RAM_MB restricts the guest kernel's usable range. Keep swapping on one CPU.

## Evidence to keep open

- [Lazy reservation and sparse test](../results/memory/screenshots/lazy-allocation.png)
- [Fork before COW](../results/memory/screenshots/fork-before-cow.png) and [COW correctness](../results/memory/screenshots/cow-validation.png)
- [Historical fork measurement](../results/memory/screenshots/fork-benchmark.png)
- [Without swap](../results/memory/screenshots/no-swap-pressure.png) and [with swap](../results/memory/screenshots/swap-pressure.png)
- [Memory-statistics observation](../results/memory/screenshots/memstat-observation.png)
- [FIFO](../results/memory/screenshots/fifo-hotcold.png) and [Clock](../results/memory/screenshots/clock-hotcold.png)
- [Full regression pass](../results/memory/screenshots/regression-tests.png)

The full suite can take much longer than a short presentation, especially under exhaustion. Show an already completed record as historical evidence and offer a fresh run separately. Never present a screenshot as a live run. Custom cowtest/lazytest success is not an official MIT grader result.

The most useful source locations are `kernel/pgfault.c`, `kernel/vm.c`, `kernel/kalloc.c`, `kernel/swap.c`, `kernel/pipe.c` and the `wait` path in `kernel/proc.c`. The [feature page](../features/memory.md) links to those files. Keep limitations visible: one-CPU eviction, fixed swap capacity, limited trials and untraced residual-slot ownership.
