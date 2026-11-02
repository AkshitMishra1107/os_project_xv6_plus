# Matched comparison study

Actual QEMU experiments on 2 November 2026. These support the comparison justifications and numeric tables in the final report. The implementation remains unchanged at `09d752e`; the tested source tree is `3341e82`.

| Experiment | Reference | Extended | Interpretation |
|---|---|---|---|
| Five forkbench samples | 40, 43, 44, 46, 47 ticks; median 44 | 8, 7, 6, 6, 8 ticks; median 7 | 84.1% fewer median guest ticks for this 50-cycle loop |
| Three fresh-boot hotcold samples | FIFO total transfers 16013, 16013, 16013 | Clock total transfers 15375, 15375, 15375 | 3.98% fewer median paging transfers in this measured phase |
| Same 200 MiB request | Eager allocation fails | Lazy reservation succeeds | Deferred backing, not additional physical RAM |
| Same 80 MiB COW workload | Eager fork fails | Checked isolation passes | Shared initial backing and private writes |
| Same 12 MiB workload at 8 MiB managed limit | Before-swap child status -1 | FIFO and Clock complete checked pages with status 0 | Additional supported workload capacity through disk backing |

Percentage reduction is `100 * (reference - extended) / reference`. The successful 12 MiB input is 50% above the fixed 8 MiB managed range; this is workload arithmetic, not a measured maximum capacity or increased RAM. The policy transfer reduction does not measure elapsed-time speedup. Five sequential fork samples and three guest boots per policy do not establish general statistical performance. No host-cache flush or randomized policy ordering was performed.

## Method and source controls

- Earlier eager checkpoint: `v0.0`, `24d95707e371f5bba684c601f7841f7488a17ccc`. Only the identical project `cowtest.c` and `forkbench.c` commands and their Makefile registration were added to the isolated earlier tree. Its kernel was unchanged.
- Before-swap checkpoint: `c325f8f0e6e05267684812f5f974adc6734f5ee9`. Its `memhog.c` is byte-identical to the extended workload.
- Normal comparisons: one CPU, 128 MiB managed RAM. Paging comparisons: one CPU, 8 MiB managed range, 128 MiB exposed by QEMU. FIFO is policy 1; Clock is policy 2. Clean builds separate changed configurations.
- Forkbench touches 20 MiB before timing fifty sequential fork/exit/wait cycles. Five measurements were made within each version's guest boot.
- Each policy's three hotcold measurements began in a fresh guest boot without preceding user workloads. The program excludes initialization from its reported transfer deltas. The same pages can transfer repeatedly.
- Final focused thread, synchronization, tracing, signal, debugger and combined checks used three CPUs and 128 MiB. The full inherited `usertests.c` run completed with `ALL TESTS PASSED`; [full original command output](outputs/full-usertests.txt) is retained.
- This fresh racetest produced 200000 both without and with a mutex. It did not expose a lost update in that execution; an occasional matching result does not make the unprotected source safe. No counter-error improvement percentage is inferred from it. The short speedup workload produced 1, 0, 1 ticks, insufficient for a reliable scaling claim.
- Pressure runs retained 2 FIFO and 2 Clock swap slots. Ownership was not audited; these counters alone prove neither a leak nor leak freedom. Earlier timed-out one-CPU full-suite evidence remains explicitly incomplete.

## Inputs outputs and screenshots

The screenshots show a local read-only viewer of the actual recorded QEMU commands and outputs. They are not photographs of a personal terminal or personal browser session. Version combinations are labelled as separate recorded sessions; shortened excerpts mark omissions. The text records, source hashes, all samples, image hashes and configuration metadata are in [measurements.json](measurements.json).

- Earlier and extended allocation and fork outcomes: [capture](screenshots/memory-comparison.png) and [recorded output](outputs/memory-comparison.txt).
- Same forkbench five repetitions in each version: [capture](screenshots/fork-comparison.png) and [recorded output](outputs/fork-comparison.txt).
- Same pressure inputs before and after disk paging: [capture](screenshots/pressure-comparison.png) and [recorded output](outputs/pressure-comparison.txt).
- FIFO fork isolation shared paging and counters: [capture](screenshots/paging-fifo.png) and [recorded output](outputs/paging-fifo.txt).
- Clock pressure fork isolation and shared paging: [capture](screenshots/paging-clock.png) and [recorded output](outputs/paging-clock.txt).
- FIFO hotcold three fresh boot observations: [capture](screenshots/policy-fifo.png) and [recorded output](outputs/policy-fifo.txt).
- CLOCK hotcold three fresh boot observations: [capture](screenshots/policy-clock.png) and [recorded output](outputs/policy-clock.txt).
- Kernel thread memory futex and library checks: [capture](screenshots/thread-api.png) and [recorded output](outputs/thread-api.txt).
- Shared results synchronization and lifecycle: [capture](screenshots/shared-results.png) and [recorded output](outputs/shared-results.txt).
- Fixed workload one two and three workers: [capture](screenshots/parallel-timing.png) and [recorded output](outputs/parallel-timing.txt).
- Earlier image has no added project commands: [capture](screenshots/baseline-interfaces.png) and [recorded output](outputs/baseline-interfaces.txt).
- Selected exec tracing and worker mask inheritance: [capture](screenshots/trace.png) and [recorded output](outputs/trace.txt).
- Signal cases debugger primitives and lifetime: [capture](screenshots/signals-debug-life.png) and [recorded output](outputs/signals-debug-life.txt).
- Debugger breakpoints known callers and resumed result: [capture](screenshots/debugger.png) and [recorded output](outputs/debugger.txt).
- Real workers lazy pages signal and memory snapshots: [capture](screenshots/combined.png) and [recorded output](outputs/combined.txt).
- Memory snapshots around sparse lazy allocation: [capture](screenshots/memstat-lazy.png) and [recorded output](outputs/memstat-lazy.txt).
- Final SMP complete inherited regression suite: [capture](screenshots/full-suite.png) and [recorded output](outputs/full-suite.txt).
