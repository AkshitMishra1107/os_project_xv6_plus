# Current validation and comparative results

[Download the complete evidence archive](evidence.zip) for all raw console records, source manifests, 95 timing cases, exact traces, models and native framebuffers. This page keeps summary data and selected screenshots directly browsable. The directory map below describes the archive contents.

Kernel and workloads: source checkpoint `7f925b5`, following ownership/integrity commit `722339e`. Experiments completed on 2–3 November 2026 using the common toolchain recorded under data. The subsequent documentation commits preserve those tested sources. All new screenshots are native QEMU virtual-console captures. Full raw console bytes and per-command output are under outputs; the manifest hashes retained files.

## Full regression and ownership

| CPUs | Policy | Full usertests | Swap outs | Swap ins |
|---|---|---|---|---|
| 1 | FIFO | PASS | 82514 | 692 |
| 1 | Clock | PASS | 81831 | 5 |
| 1 | Random | PASS | 82522 | 690 |
| 3 | FIFO configured, eviction disabled | PASS | 0 | 0 |

The inherited usertests source is unchanged. One-CPU owner audits reconcile every bitmap slot against the unique live page tables. Remaining slots belong to sleeping `init` at VAs 0 and 0x1000; orphan, missing, duplicate and invalid counts are zero. Raw records retain the individual slot IDs. Full-suite jobs ran concurrently; their host completion durations are not a policy speed comparison.

All pressure policies pass `memhog 4`, full-byte `memhog 12`, `swapfork`, `threadswap`, and the three-worker `finaldemo 4` with active paging. The host pattern negative control detects a corrupted byte in each of four disk blocks. Focused SMP checks cover thread/lifetime/shared-VM, futex/library, race/queue/semaphore/lock-order, tracing, signals and debugger primitives/abandonment.

## Paging phase

Each row summarizes five paired seeds with 6000 references after filling 12 MiB. Fresh guests and pristine images are used, policy order rotates, and final full-page verification is outside the timer. Every checksum matches the independently generated reference stream and every bad count is zero. JSON/CSV retain every sample and timing range.

| Pattern | Policy | Median ms | Median recovery calls | Median in / out |
|---|---|---|---|---|
| seq | FIFO | 27756.0 | 6008 | 6008 / 6018 |
| seq | Clock | 22417.3 | 5535 | 5535 / 5542 |
| seq | Random | 18480.7 | 4160 | 4160 / 4165 |
| uniform | FIFO | 12360.3 | 2521 | 2521 / 2533 |
| uniform | Clock | 11255.2 | 2527 | 2527 / 2533 |
| uniform | Random | 11571.3 | 2525 | 2525 / 2533 |
| hot | FIFO | 1857.8 | 373 | 373 / 374 |
| hot | Clock | 1155.7 | 247 | 247 / 255 |
| hot | Random | 1753.7 | 349 | 349 / 357 |
| phase | FIFO | 2507.5 | 494 | 494 / 493 |
| phase | Clock | 1835.0 | 370 | 370 / 374 |
| phase | Random | 2435.1 | 482 | 482 / 493 |

Counts measure recovery and disk-transfer work; elapsed time also includes selection and emulator costs. Interpret the ranking per pattern. Independent FIFO/Clock/LRU/OPT models replay the same streams at a fixed 1700-frame budget, separately from the guest's changing availability. The standard Belady trace self-check gives FIFO 9 faults at three frames and 10 at four. Windowed working-set measurements and exact trace hashes are retained.

## Same-source COW ablation

Five fresh trials per setting, one CPU, 128 MiB, 20 MiB fully touched parent and ten fork/exit/wait cycles. Only FORK_COW changes. The write-all child verifies every overwritten byte and the parent verifies unchanged data.

| Case | Eager median ms | COW median ms | COW change |
|---|---|---|---|
| readonly child | 1079.3 | 107.0 | 90.1% less time |
| writeall child | 2050.4 | 3097.8 | 51.1% more time |

Read-only children measure avoided copying; write-all children expose deferred-copy fault cost.

## Equal-total-work parallel scaling

Five fresh trials per worker count on the same three-CPU configuration. One global range of 400 million iterations is partitioned, with checksum 1914453504 in every case. The counter frequency is independently verified from QEMU's device tree as 10 MHz. Timings are emulated guest elapsed time.

| Workers | Median ms | Speedup relative to one worker |
|---|---|---|
| 1 | 1046.3 | 1.00x |
| 2 | 557.6 | 1.88x |
| 3 | 401.5 | 2.61x |

`racetest` deliberately forces a common read-before-write interleaving: expected 4000, unsafe 1000, mutex 4000. It demonstrates lost updates and protection, rather than spontaneous race frequency.

## Evidence map

- `outputs/full`, `outputs/smoke`, `outputs/bench`: original console bytes, per-command records, source/configuration/kernel hashes.
- `outputs/historical`: earlier eager/pre-swap and initial newer-upstream probes.
- `data`: all samples, exact traces, independent models, environment, corruption control and per-file diff statistics.
- `screenshots`: native QEMU framebuffer images, grouped by suite and case.
- `reproduce`: runner and independent-check sources; canonical copies live in tools/.

Use the [runner instructions](../../../tools/README.md) to reproduce. Earlier [comparison-study](../comparison-study/README.md) results remain historical; current benchmarks replace its coarse timing and single-pattern figures as the primary comparison.
