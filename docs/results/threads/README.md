# Threads: recorded validation

## v0.6 kernel threads

Configuration: QEMU, three CPUs, 128 MB, FIFO setting. Eviction is disabled in SMP builds. All outputs below are copied from the actual console run; no terminal screenshots are synthesized.

- `kthreadtest`: 10/10 passes. Covers seven-worker limit, shared PID, invalid joins, TID reuse over 50 create/join cycles, threaded fork isolation, and exec/shrink guards.
- `threadlife`: 10/10 passes. A worker's process exit produces status 3; external kill terminates the complete group with status -1.
- Full `usertests`: `ALL TESTS PASSED`.
- Final `memstat`: 32,304 free pages, zero swap slots. This is a snapshot, not a proof about every possible workload.

[First lifecycle outputs](outputs/v06-lifecycle.txt) · [Full regression output](outputs/v06-usertests.txt)

The private run metadata records the parent commit and working source diff because the tests precede the milestone commit. Reproduce from this milestone with a clean build: `make clean && make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1`, then run `kthreadtest`, `threadlife`, `usertests` and `memstat` at the xv6 prompt.
