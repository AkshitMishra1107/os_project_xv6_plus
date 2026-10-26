# xv6 Plus implementation and results comparison plan

Created: 22 October 2026. The user requested comparisons of the original implementation with the project's changes, for the final report and viva.

## Reference versions

- Initial upstream checkout: `f780d78`, branch `riscv`. It already has an eager/lazy allocation mode and passed `usertests`. Use this as setup evidence, not as evidence that the team implemented lazy allocation.
- Historical project starting revision selected: `c4de4b0285e1c8e5348bdd31f597fe958f37b912`, before upstream lazy allocation. Local baseline: `24d95707e371f5bba684c601f7841f7488a17ccc`, tag `v0.0`, branch `v0.0-baseline`; the full export corrects an earlier screenshot transcription. Lazy checkpoint: `f41e293`, tag `v0.1`, branch `v0.1-lazy` at that milestone. See `10-git-checkpoints-before-cow.png` and `v0.0-commit.txt`.
- Compatibility adjustment on the historical baseline: explicit `-march=rv64gc` for C and assembly; preserve this in both sides of performance comparisons.
- The `baseline` program has been compiled and run. The user supplied `baseline: sbrk(200MB) failed (expected on stock xv6)`; this is the first actual allocation before result.

## Compare implementation and behavior

| Milestone | Starting mechanism to document | Proposed project change | Code to compare | Evidence to collect |
| --- | --- | --- | --- | --- |
| v0.1 lazy allocation | Eager `sys_sbrk` calls `growproc`, which allocates/maps physical pages during growth | Grow the valid address range; allocate and zero a page when first accessed; support faults during kernel/user copies and sparse mappings | `sys_sbrk`, `growproc`, `uvmunmap`, `uvmcopy`, `usertrap`, copy functions, new `handle_page_fault` | Actual `baseline` output before/after, sparse allocation `lazytest`, zero-filled pages, read into untouched buffers, test suite results |
| v0.2 copy-on-write | `uvmcopy` allocates physical pages and copies parent data for the child | Share read-only mappings with reference counts; resolve a write by copying one shared page or restoring write permission when exclusively owned | `uvmcopy`, allocator/reference counts, fault handler, `copyout` behavior, PTE flags and TLB flushes | `cowtest` before/after, parent/child isolation, identical `forkbench` workload and actual tick counts, cleanup behavior |
| v0.3 memstat | No project memory-statistics syscall or command | Expose allocation/fault/COW/swap counters and current resource counts | Shared structure, kernel counters, syscall wiring, user command | Actual counter output and free pages before/after each workload; do not record unavailable baseline metrics as zero |
| v0.4 swap | Private user pages cannot be moved to the project's reserved swap area; physical allocation can fail under pressure | Track frames and slots, evict eligible private pages, restore swapped pages on faults, release RAM/slots on cleanup | Swap module, user allocation path, fault handler, unmapping, fork, prefault/copy paths, locking, build settings | `memhog 12` with `RAM_MB=8`, corruption counts, swap counters, `swapfork`, free-page/slot cleanup, final correctness tests |
| Replacement policies | FIFO chooses the oldest loaded eligible frame | Clock gives recently accessed frames a second chance | Victim selection and handling of the accessed bit | Compare actual `hotcold` and `memhog` counts under FIFO and Clock with identical settings |

This table describes the intended comparisons. v0.1 through v0.3 are validated and saved with exported source diffs. The COW benchmark changes from 38 ticks before to observed 3–4 ticks after. Preserve the initial failures and fix history. The v0.3 statistics command works and its full suite passes; free pages read 32,500 before/after targeted workloads and the complete suite. Memstat is saved at `712951e42df27676a9dd88fb76264b51402dc1cd`, tag v0.3. Current branch is v0.4-swap. The constrained-memory no-swap reference and first after-swap comparison are now recorded: the unchanged 12 MB workload changes from child status -1 to 3,072 checked pages, zero bad pages, and child status 0. Full swap validation and policy comparison remain pending.

## Evidence required for each release

1. Record the starting commit, branch, build flags, RAM, CPU count, selected policy, and workload parameters.
2. Preserve relevant original functions and a complete change diff, including new files, using Git commits/tags when the user reaches that step.
3. Explain the original behavior, the changed behavior, why the change is needed, and the relevant data structures/control flow.
4. Collect an actual before result before changing the feature, then rerun the same workload after the change.
5. Save screenshots, numerical results, and the exact commands used. State correctness and performance results separately.
6. Record remaining limitations and unexpected failures along with their resolution or pending status.

For general comparisons use the same CPU count and RAM on both sides. COW tests need the guide's default 128 MB setup; swap demonstrations use one CPU and 8 MB usable RAM. Compare FIFO and Clock using the same RAM, CPU count, workload, and initial conditions, with a clean build after changing build-time flags.

Only calculate improvements from measured values. Do not claim that Clock outperforms FIFO until the data demonstrates that result. Initial setup screenshots are separate from feature comparison results.

## Collected results

| Checkpoint | Actual result | Evidence |
| --- | --- | --- |
| Initial upstream test suite | `ALL TESTS PASSED` | `02-baseline-usertests.png` |
| Historical baseline build before ISA adjustment | Build completed; guest startup failed to reach the first banner | `03-eager-baseline-build.txt`, `04-boot-registers-before-fix.txt` |
| Historical baseline boot after explicit ISA flags | Reached xv6 shell, 128 MB RAM, one CPU | `05-boot-after-isa-fix.png` |
| Historical baseline test suite | `ALL TESTS PASSED` | `06-prelazy-usertests.png` |
| Eager 200 MB allocation | `baseline: sbrk(200MB) failed (expected on stock xv6)` | `03-eager-baseline-result.txt`; a dedicated screenshot is optional additional evidence |
| v0.1 integration | Allocation comparison, targeted lazy test, and full regression suite pass; checkpoint `f41e293` tagged `v0.1`; complete source diff exported | `07-lazy-allocation-results.png`, `08-v0.1-usertests.png`, `10-git-checkpoints-before-cow.png`, `v0.1-lazy-allocation.patch` |
| Original cleanup behavior | Screenshot confirms `uvmunmap` panics on a missing PTE or invalid mapping before the lazy-allocation edit | `v0.1-uvmunmap-before.png` |
| Lazy 200 MB allocation | `baseline: sbrk(200MB) succeeded`, compared with the recorded eager failure | `07-lazy-allocation-results.png` |
| Sparse 256 MB reservation | `lazytest: OK (656 pages touched)`; includes the instructed zero-page and syscall-buffer checks | `07-lazy-allocation-results.png`; sparse loop count is not a measurement of total resident pages |
| Full v0.1 regression suite | `ALL TESTS PASSED` | `08-v0.1-usertests.png` |
| Before COW: memory-pressure fork | `cowtest: fork failed`; current lazy-allocation kernel still copies mapped pages eagerly at fork; 128 MB RAM, one CPU | `09-before-cow.png` |
| Before COW: fork timing | `forkbench: 50 forks of a 20 MB process took 38 ticks`; one measured run, 128 MB RAM, one CPU | `09-before-cow.png` |
| After COW: memory-pressure fork | `cowtest: OK`, compared with the recorded before-COW fork failure | `11-cow-results-forkbench-exec-failure.png` |
| After COW: lazy-allocation compatibility | `lazytest: OK (656 pages touched)` | `11-cow-results-forkbench-exec-failure.png` |
| After COW: initial fork benchmark launch | `exec forkbench failed`; `ls` confirms executable file presence (35,288 bytes). Original cause remains unconfirmed | `11-cow-results-forkbench-exec-failure.png`, `12-forkbench-file-present.png` |
| After COW: fresh-boot fork timing | `forkbench: 50 forks of a 20 MB process took 3 ticks`; one measured run, 128 MB RAM, one CPU | `13-cow-forkbench-fresh-boot.png` |
| After COW: repeat after cowtest | `cowtest: OK`, then benchmark succeeds in 3 ticks, then `lazytest: OK (656 pages touched)`; 128 MB RAM, one CPU | `14-cow-repeat-targeted-results.png` |
| After COW: benchmark after lazytest | Another `forkbench` succeeds in 3 ticks after the passing lazy test | `15-v0.2-usertests-sbrkfail.txt` |
| Initial full COW regression suite | **Failed:** `test sbrkfail: FAILED`, followed by `SOME TESTS FAILED`; subsequently passes after the guide's wait-order change | `15-v0.2-usertests-sbrkfail.txt` |
| After wait fix: isolated failing test | `usertests sbrkfail` reports `test sbrkfail: OK` and `ALL TESTS PASSED` for this isolated run | `17-cow-sbrkfail-fix-targeted-pass.png` |
| After wait fix: targeted tests and timing | `cowtest` and `lazytest` pass; benchmark takes 3 ticks after cowtest and 4 ticks after lazytest | `17-cow-sbrkfail-fix-targeted-pass.png` |
| Final full COW regression suite | `ALL TESTS PASSED` | `18-v0.2-full-usertests-pass.png` |
| COW Git release and source comparison | Commit `87437a019947f47f8f811dde33b31757f01d9091`, tag `v0.2`; complete source diff exported; current branch `v0.3-memstat` | `19-v0.2-checkpoint-v0.3-branch.png`, `v0.2-cow.patch`, `v0.2-commit.txt` |
| Initial v0.3 memstat readings | Before tests: faults 4, lazy 2, COW copies 2, free pages 32,500. After cowtest: 20,518 / 20,486 / 27 / 32,500. After lazytest: 21,188 / 21,148 / 31 / 32,500. All swap values zero | `20-v0.3-memstat-results.png`, `v0.3-summary.md` |
| Full v0.3 regression and post-suite statistics | ALL TESTS PASSED; faults 746,476, lazy allocations 694,850, copies 174, free pages 32,500; all swap values zero | `21-v0.3-full-suite-and-memstat.png` |
| v0.3 Git release | Commit 712951e42df27676a9dd88fb76264b51402dc1cd, tag v0.3; source diff exported; v0.4-swap branch created | `22-v0.3-checkpoint-v0.4-branch.png`, `v0.3-memstat.patch`, `v0.3-commit.txt` |
| Constrained-memory setup, no swap yet | Three successful memstat readings show free pages 1,870 and zero swap values. Counter sequences: faults 10/16/22, lazy 4/6/8, copies 4/6/8. Consistent with the instructed 8 MB usable xv6 RAM limit | `23-8mb-memstat-entry-and-retries.png` |
| Disk reservation | fs.img size confirmed as 18,825,216 bytes, matching the 2,000-block filesystem plus 16,384 reserved 1 KB blocks | `24-swap-disk-reservation-size.png` |
| No-swap 4 MB control | 1,024 pages verified, 0 bad, child status 0; free pages recover to 1,870 | `25-no-swap-pressure-baseline.png` |
| No-swap 12 MB pressure | Child status -1 before completion; no full-data verification claimed; free pages recover to 1,870 and swap fields stay zero | `25-no-swap-pressure-baseline.png` |
| No-swap source checkpoint | c325f8f0e6e05267684812f5f974adc6734f5ee9; source comparison exported at 2,855 bytes | `before-swap.patch`, `before-swap-commit.txt` |
| First after-swap 4 MB control | 1,024 pages checked, zero bad, child status 0; zero swap counters; free pages recover to the new engine baseline of 1,856 | `28-fifo-swap-memhog-results.png` |
| First after-swap 12 MB pressure | 3,072 pages checked, zero bad, child status 0; program reports cumulative 4,369 swap outs/3,072 ins; following memstat reads 3,076 ins, 1,857 free pages, one used slot | `28-fifo-swap-memhog-results.png`, `v0.4-summary.md` |
| Repeated swap pressure and resource snapshots | All three 12 MB runs verify 3,072 pages with zero bad pages/status 0. Free pages remain 1,857 and used slots one after each; cumulative final outs/ins 13,107/9,228 | `29-fifo-repeat-pressure-results.png` |
| COW and swap interaction | 6 MB swapfork: child bad=0 and parent OK. COW copies 2 to 1,543; swap outs/ins 0/0 to 2,839/1,540. Final free pages/used slots 1,857/one | `30-fifo-swapfork-results.png` |
| FIFO hotcold measured phase | Fresh-boot first run: 12 MB, three passes; 8,006 swap ins and 8,007 swap outs after initialization. Final cumulative ins/outs 8,010/9,299 | `31-fifo-hotcold-results.png` |
| Clock hotcold measured phase | Fresh-boot first run: 7,691 swap ins and 7,684 swap outs; 315 fewer ins (3.9%) and 323 fewer outs (4.0%) than the FIFO measured phase. No timing measurement | `32-clock-hotcold-results-and-command-retry.png` |
| Clock targeted correctness | memhog 4/12 zero bad/status 0, swapfork child bad=0/OK, lazytest OK (656 pages touched); pre/post free pages/used slots 1,857/one | `33-clock-correctness-results.png` |
| First full v0.4 regression | Failed: countfree write failure, then 31,876 counted versus 32,159 initially (reported deficit 283). Visible individual tests report OK; cause/actual leak unconfirmed. Instructed 128 MB/one CPU/FIFO | `34-v0.4-usertests-countfree-failure.png` |
| countfree source investigation | Counter counts delivered pipe bytes and ignores child status; pipewrite copies user data under its spinlock. Kernel-staged read/write repair instructions preserve the source test/checks | `35-countfree-driver-pipewrite-source.txt` |
| Focused validation after pipe repair | usertests pipe1 and driver countfree checks pass; free pages 32,306 to 32,307, used slots zero to one. This is an isolated run | `36-pipe-staging-focused-test-pass.png` |
| Complete v0.4 regression after repair | Full usertests ALL TESTS PASSED from fresh boot; free pages 32,306 to 32,307, slots zero to one; original failure preserved | `37-v0.4-fresh-boot-and-usertests-start.png`, `38-v0.4-full-usertests-pass-and-memstat.png` |
| Final integrated-kernel demos | baseline 200 MB reservation succeeds; lazytest/cowtest pass; forkbench six ticks. Free pages/used slots stay 32,307/one after every demo; no additional swap activity | `39-v0.4-baseline-lazy-cow-demo.png`, `40-v0.4-final-forkbench-and-resource-recovery.png` |
| Local v0.4 preservation | Commit 93fa86df3ddda8691cc2c260b373dd58f726620f, tag v0.4, branch v0.4-swap, clean working tree; full/engine/pipe source exports checked | `41-v0.4-source-staging-and-statistics.png`, `42-v0.4-commit-tag-exports-clean-status.png`, three v0.4 patches and identifier files |
| Captured environment | Ubuntu 26.04.1 LTS, WSL2 kernel 6.18.54.1, Git 2.53.0, GCC 15.2.0, QEMU 10.2.1, GDB 17.1 | `43-environment-versions.png`, `environment-versions.txt` |
| Report follow-up | Final document assembly when requested; retain original failure, residual-slot uncertainty, pre-pipe-repair policy-run context, and configuration-evidence limitations | Awaiting user instruction |

The before-COW screenshot confirms both measurement programs were included in the filesystem and run on the v0.1 kernel, with QEMU arguments `-m 128M -smp 1`. In the instructed `cowtest`, 80 MB is touched before fork; eagerly duplicating those pages requires approximately another 80 MB plus kernel and other process memory, exceeding this configuration. The observed fork failure is the expected before result. The initial after-COW screenshot shows `cowtest: OK` and a passing lazy test; that screenshot does not show emulator arguments. The subsequent fresh-boot benchmark screenshot explicitly confirms `-m 128M -smp 1`.

`forkbench` touches 20 MB and measures 50 sequential fork/wait cycles. The before-COW observation is 38 ticks. After-COW observations include 3 ticks, while the final post-wait-fix sequence reports 3 then 4 ticks. Report the observed after range as 3–4 ticks: 34–35 fewer ticks, approximately 89.5–92.1% less measured tick time compared with the single 38-tick before run. These are individual measurements with a coarse timer and different preceding workloads, not controlled averages or a general speedup guarantee.

The first after-COW benchmark launch failed despite the file being present; it did not recur in the later shown sequences. Its original cause remains unconfirmed. The first full COW suite failed at `sbrkfail`. After applying the guide's later wait-order change earlier in the project, both isolated `sbrkfail` and the full suite pass. This verifies the practical fix and supports the memory-pressure/COW status-copy diagnosis; no internal allocation trace was collected. The validated v0.2 release is now committed/tagged with its full identifier and source diff preserved.

The final integrated v0.4 kernel, after the pipe repair/full suite, records six ticks for the same forkbench workload: 32 fewer ticks (84.2%) than the single original 38-tick observation. Keep this result separate from v0.2's 3–4 tick measurements. Different preceding work and additional kernel code were present; no controlled profiling established why the timing differs. Use measured tick reduction rather than claiming a general OS speedup.
