# Threads: recorded validation

## v0.6 kernel threads

Configuration: QEMU, three CPUs, 128 MB, FIFO setting. Eviction is disabled in SMP builds. All outputs below are copied from the actual console run; no terminal screenshots are synthesized.

- `kthreadtest`: 10/10 passes. Covers seven-worker limit, shared PID, invalid joins, TID reuse over 50 create/join cycles, threaded fork isolation, and exec/shrink guards.
- `threadlife`: 10/10 passes. A worker's process exit produces status 3; external kill terminates the complete group with status -1.
- Full `usertests`: `ALL TESTS PASSED`.
- Final `memstat`: 32,304 free pages, zero swap slots. This is a snapshot, not a proof about every possible workload.

[First lifecycle outputs](outputs/v06-lifecycle.txt) · [Full regression output](outputs/v06-usertests.txt)

The private run metadata records the parent commit and working source diff because the tests precede the milestone commit. Reproduce from this milestone with a clean build: `make clean && make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1`, then run `kthreadtest`, `threadlife`, `usertests` and `memstat` at the xv6 prompt.

## v0.7 futexes

Same three-CPU, 128 MB configuration. `futextest` passed 10/10 runs: 2,000 total handshakes, mismatch returns, invalid addresses, colliding-bucket isolation, cross-process isolation, and group-kill cleanup. The raw thread/lifecycle tests, `lazytest` and `cowtest` also passed. [Selected actual outputs](outputs/v07-futex.txt).

## v0.8 library

Three CPUs, 128 MB. `threadtest` and `libtest` each passed 10/10 runs. Each library run performs 60 nested create/join pairs, a 3,000-increment mutex check, a condition-variable barrier and a semaphore capacity check. Both returning from a worker and explicit thread exit are exercised. Final focused raw thread/futex/lifecycle checks passed. Free pages were 32,300 before and after; swap slots stayed zero. [Selected actual outputs](outputs/v08-library.txt).

## v0.9 final implementation validation

These are executed observations on the final source snapshot. Each run records its checkout commit and source hashes; some initial runs used working changes before the milestone commit. The original validation ran on `feature/threads-implementation`; PR #4 now preserves those commits in main. See the integration record below for the post-merge source and runtime checks. The tests preserve the complete source manifest privately; the public [validation index](validation.json) identifies its SHA-256, checkout commit, console hashes, configurations and command counts.

| Build | Executed validation | Observed result |
|---|---|---|
| 3 CPUs, 128 MB, FIFO setting | Ten runs each of kthreadtest, threadlife, futextest, threadtest, libtest, racetest, prodcons, philosophers, semtest, exittest, threadmem, and each speedup case; full usertests | All passed |
| 1 CPU, 128 MB, FIFO | Baseline, lazy/COW/fork benchmark, raw thread/futex/library/lifecycle/memory tests, full usertests | All passed |
| 1 CPU, 8 MB, FIFO | 4 MB control, two 12 MB memhog runs, swapfork and ten threadswap runs | Zero bad pages; all passed |
| 1 CPU, 8 MB, Clock | 4 MB control, two 12 MB memhog runs, swapfork and three threadswap runs | Zero bad pages; all passed |

SMP free-page counts were 32,300 at the start, 32,300 after each complete demonstration round, and 32,300 after full usertests. Swap slots stayed zero in that configuration. CPU-1 free pages and residual slots are recorded in the pressure output and validation index; a stable residual slot is not asserted to be a leak or proved to be leak-free by this observation.

The ten unprotected race results were 183587, 109036, 185490, 200000, 90225, 200000, 132290, 200000, 109920, 119648. Protected results were 200,000 in every run. An unprotected run can reach the expected value by scheduling chance; that is not evidence of mutual exclusion.

### Timing limits

The fixed 40-million-iteration workload produced these ten guest tick observations per case:

| Workers | Recorded ticks |
|---|---|
| 1 | 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 |
| 2 | 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 |
| 3 | 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 |

These measurements are too coarse to establish proportional speedup. Every workload checksum passed. Emulation, scheduling and tick quantization limit this experiment; CPU count alone is not a performance result.

### Selected console records

- [One complete demonstration round](outputs/v09-demonstrations.txt)
- [Full three-CPU usertests](outputs/v09-smp-usertests.txt)
- [Full one-CPU usertests](outputs/v09-onecpu-usertests.txt)
- [FIFO control, pressure and shared-thread results](outputs/v09-fifo-pressure.txt)
- [Clock control, pressure and shared-thread results](outputs/v09-clock-pressure.txt)

These files contain actual console text. The one-CPU transcript includes two Ctrl+P process snapshots; its capture window was extended without restarting the guest. Earlier original screenshots remain in the memory results folders; no new terminal screenshots were synthesized for this automated run. Full repeated transcripts and build logs remain in the private report archive.

### Reproduce

In Ubuntu/WSL, run `make clean` and `make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1`. At the xv6 prompt run the programs listed above individually, then `usertests` and `memstat`. Exit QEMU with Ctrl+A then X. For replacement tests, rebuild clean with `CPUS=1 RAM_MB=8 SWAP_POLICY=1` (FIFO) or `SWAP_POLICY=2` (Clock), then run `memhog 4`, `memhog 12`, `swapfork`, `threadswap`, and `memstat`. The host QEMU command exposes 128 MB physically; `RAM_MB` restricts the RAM managed by xv6.

## Main integration

PRs 3 and 4 are merged. Their earlier branch-validation records remain historical evidence; see the [integrated-main check and release record](integration.md) for source equivalence and the post-merge smoke run.
