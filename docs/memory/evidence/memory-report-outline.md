# Memory report outline and saved steps

Prepared: 23 October 2026 (Asia/Calcutta).

This file organizes the user's completed memory-management work for the upcoming report. The detailed chronological record is progress-log.md. Measurements and comparisons are in comparison-plan.md, with supporting screenshots, source diffs, and milestone summaries in this folder. This outline is not a final report and does not mark pending work complete.

## Ordered steps already recorded

1. Installed and launched Ubuntu under WSL on Windows, created the Linux account, and installed Git, build tools, the RISC-V compiler/binutils, GDB, QEMU packages, and tmux. Exact capture-date environment versions were later saved on 26 October; see item 42.
2. Fixed the initially missing RISC-V QEMU command by installing the separate qemu-system-riscv package. QEMU 10.2.1 and GDB 17.1 were subsequently evidenced.
3. Kept the project in the requested Mini Project folder and cloned MIT xv6-riscv into xv6-plus.
4. Fixed the initial Makefile file-prefix-map argument by quoting its value, because the Windows project path contains a space. The original screenshot and exact patch are saved.
5. Built and booted initial upstream f780d78 and ran its full test suite successfully. Recognized that this upstream already contained lazy allocation, so it is setup evidence rather than the team's lazy implementation.
6. Selected historical starting revision c4de4b0285e1c8e5348bdd31f597fe958f37b912, before upstream lazy allocation, to implement the guide's memory features ourselves.
7. Investigated the historical guest's boot hang with QEMU's register monitor. Saved the PC-zero register dump and ELF attributes. Explicit rv64gc C/assembly flags and a clean rebuild produced a working boot. The exact original faulting instruction was not identified.
8. Added the baseline program and recorded the eager 200 MB sbrk failure on a 128 MB configuration. The historical baseline full suite passed. Saved local checkpoint v0.0.
9. Implemented lazy sbrk growth, a new page-fault handler, user-trap routing, sparse mapping cleanup/fork support, and fault-aware kernel/user copy helpers. Added lazytest and its build entry.
10. Recorded the 200 MB request now succeeding and lazytest passing with 656 explicitly sparse-write-touched pages. The full v0.1 suite passed. Saved commit/tag v0.1 and later exported its complete source diff.
11. Added before-COW comparison programs. With 128 MB and one CPU, cowtest's eager fork failed after touching 80 MB; forkbench's 50 fork/wait cycles with a 20 MB process took 38 ticks.
12. Preserved the comparison programs in commit 28c7475 and created the COW branch. Added PTE software flags, reference-counted physical allocation/freeing, shared-page fork mappings, and COW write recovery. Existing copyout permission checks invoke the same recovery path.
13. Saved an initial exec forkbench failed result and confirmed the executable was present. A fresh boot and later repeated sequences completed successfully. Preserve the failure without assigning an unproven cause.
14. Recorded cowtest and lazytest success and faster observed fork benchmarks. The first full COW suite failed at sbrkfail. Saved its output and the user's exact test/fault-handler excerpts.
15. Brought forward the guide's later wait change: save child status, free the child, release locks, then copy status to the parent's memory. The isolated sbrkfail test and complete suite subsequently passed. Final post-fix benchmarks took 3 and 4 ticks.
16. Saved COW release v0.2, full revision identifier, and v0.1/v0.2 source comparisons. Created v0.3-memstat.
17. Added the shared memstat structure, initial statistics module, declarations/build entry, event increments, syscall wiring, and user command. The guide/default syscall number instructed was 22, conditional on it being unused; its final saved header will be captured in the v0.3 diff.
18. Recorded initial, after-cowtest, and after-lazytest statistics. Both tests passed, counters increased, and free pages were 32,500 at all three snapshots. All swap fields remained zero.
19. Ran the complete v0.3 usertests suite successfully, then recorded memstat again: 746,476 handler invocations, 694,850 lazy allocations, 174 copies, and 32,500 free pages. Swap values remained zero.
20. Saved release v0.3 at 712951e42df27676a9dd88fb76264b51402dc1cd, exported its source diff and full identifier, and created v0.4-swap. The exported diff confirms syscall number 22 and the event-counter additions.
21. After the constrained-memory configuration instructions, booted the guest and recorded three successful memstat readings with 1,870 free pages and zero swap activity. The user reports a spacing issue on an initial failed command and then repeated successful entries. Counter growth reflects command activity; normal trailing ASCII whitespace is not established as the cause. The image-size check and pressure workload remain pending.
22. Verified fs.img at exactly 18,825,216 bytes from the user's Ubuntu stat output. This confirms reserved disk length; swap behavior remains unimplemented and the pressure workload is next.
23. Added the guide's page-pattern workload with a parent/child status wrapper and measured the no-swap reference. memhog 4 verifies 1,024 pages with zero bad pages/status 0; memhog 12 terminates with status -1 before completion. Free pages recover to 1,870 after each case and all swap values remain zero.
24. Preserved the source reference at c325f8f0e6e05267684812f5f974adc6734f5ee9 and exported before-swap.patch. The user reports saving slot/accessed-bit definitions and the lock-nesting helper. Full engine integration and after measurements are still pending.
25. Supplied the complete swap engine instructions, confirmed the declarations visible in the user's Nano screenshot, and explained the final-free untracking insertion from the user's kfree screenshot. The user reports saving that hook. Combined page-fault handling and instruction-fault routing are the next instructed edits; no swap-capable build or after measurement is claimed.
26. The user reports saving the combined handler and instruction-fault routing. Next instructions connect uvmalloc frame registration, swapped-page cleanup, and swap-aware COW fork. They include per-page interrupt guards during removal to prevent eviction racing with cleanup in the single-CPU design. Buffer prefaulting and boot initialization are still pending, with no swap build or after measurements yet.
27. The user reports saving all three VM edits. Final integration instructions now add the bounded prefault helper, its declaration and read/write syscall calls, and boot initialization. The next user-run measurement repeats the unchanged memhog control/pressure sequence after a clean FIFO build at 8 MB usable RAM and one CPU. Build success and after measurements remain unconfirmed.
28. The first after-swap screenshot confirms boot and successful unchanged memhog 4 and memhog 12 workloads: respectively 1,024 and 3,072 checked pages, zero bad pages, and child status 0. The pressure workload reports 4,369 swap outs/3,072 ins; following memstat shows 3,076 ins, 1,857 free pages, and one used slot. Repeated cleanup checks are next; full validation and policy comparison remain pending. Build flags were instructed but are not displayed in this screenshot.
29. Repeated memhog 12 twice more in the same guest; all three runs verify 3,072 pages with zero bad pages/status 0. Post-run free pages/used slots remain 1,857/one throughout. Cumulative final counters are 13,107 outs/9,228 ins. No accumulation is observed for this sequence; the residual slot owner is unidentified. Next instructions add/register/build the guide's 6 MB swapfork test with failure checks; no interaction-test result is claimed yet.
30. Added and ran the instructed 6 MB swapfork test: child bad=0 and parent OK. Before/after memstat shows actual COW copies 2 to 1,543 and swap outs/ins 0/0 to 2,839/1,540. Post-run resources are again 1,857 free pages/one used slot. Next instructions add the guide's hotcold access pattern with failure checks and volatile accesses, register/build it, and collect a fresh FIFO run before comparing Clock. No policy result is claimed yet.
31. Recorded the first fresh-boot FIFO hotcold sequence. Its measured three-pass phase reports 8,006 swap ins and 8,007 swap outs. Final cumulative memstat reads handler 11,098, lazy 3,078, COW 6, swap outs/ins 9,299/8,010, free pages 1,857, slots one. Next instructions clean-build Clock and collect the identical first-workload sequence. Build flags are not visible in screenshot 31; no policy winner is claimed yet.
32. Recorded Clock hotcold measured ins/outs 7,691/7,684: 315 fewer ins (3.9%) and 323 fewer outs (4.0%) than the FIFO run. Initial conditions match the visible FIFO statistics. This is one measured run per policy without timing evidence. Screenshot 32 also preserves an apparent command-input error and later working statistics; free pages/used slots remain 1,857/one. Next are Clock memhog/swapfork/lazytest checks, then full regressions and the release checkpoint.
33. Clock memhog 4/12, swapfork, and lazytest all pass. Sequence pre/post free pages/used slots remain 1,857/one; final cumulative outs/ins are 16,185/12,308. Next instructions return to the guide's normal-RAM configuration (128 MB, one CPU, FIFO) for full usertests with initial/final memstat. The complete suite and final source/Git checkpoint are not yet confirmed.
34. On 24 October the first full v0.4 suite fails at its final countfree check: write failure, then 31,876 counted pages versus 32,159 initially, a reported difference of 283. Visible individual tests report OK. The cause and whether physical pages leaked are unconfirmed because the measurement helper itself failed. Next request actual memstat, countfree/driver, and pipewrite source before fixing. Preserve the failed screenshot and revalidate before tagging.
35. Inspected the user's helper/driver/pipewrite excerpts: countfree counts received bytes and ignores child exit status, while pipewrite calls copyin under the pipe lock. Instructed a kernel-staged read/write replacement so user copies happen outside the pipe spinlock, preserving the test and its comparison. Save/build and isolated memstat/usertests pipe1/memstat validation are pending; no root-cause or passing-rerun claim yet. No post-failure memstat was supplied before the guest was exited.
36. After the kernel staging instructions, usertests pipe1 reports OK and its driver/countfree checks pass. Free pages change from 32,306 to 32,307 and used slots from zero to one; final outs/ins 8,193/six. This is a focused result, not a complete-suite pass. Next run full usertests in the same guest using the latest statistics as the pre-suite reference, then collect final memstat.
37. On 25 October the user reports a laptop shutdown. Resume from the saved implementation with a fresh Ubuntu/QEMU launch and clean 128 MB/one CPU/FIFO build; collect a new initial memstat, full usertests result, and final memstat. The previous boot's counters/resource snapshot are historical. Implementation and prior evidence are not redone; no new test result is claimed.
38. The fresh-boot complete usertests suite now reports ALL TESTS PASSED after the pipe repair. Initial free pages/slots 32,306/zero become 32,307/one; final faults 803,901, lazy 751,596, COW 163, outs 82,554, ins 693. The explicit post-suite memstaty typo/correction is a separate shell-input incident. Next complete the guide's normal-RAM baseline/lazy/COW/forkbench demos and preserve the code/Git/source metadata; the original failed run remains in the report history.
39. Final normal-RAM baseline/lazy/COW demos pass; the integrated kernel's forkbench records six ticks for 50 forks of a 20 MB process. Free pages/slots remain 32,307/one and swap counters 82,554/693 after each demo. This six-tick result is distinct from earlier COW-only 3–4 ticks and is 84.2% fewer ticks than the original single 38-tick observation, without controlled causal profiling. Next request Git branch/status/diff-stat/tags and prepare the local v0.4 commit/tag/source exports. A baseline-command typo followed by correct success is recorded separately.
40. On 26 October the user reports another shutdown and asks to commit. Confirm the saved code/test evidence persists and provide local branch/tag checks, bounded source staging, commit/tag, full/engine/pipe patch exports, revision identifiers, and log/status commands. Execution and checkpoint confirmation are still pending; no Git action was performed by the assistant. The sibling report-evidence files remain separately preserved.
41. Local v0.4 checkpoint is confirmed at 93fa86df3ddda8691cc2c260b373dd58f726620f, tag v0.4, branch v0.4-swap, clean tree. The commit changes 14 files (693 insertions/79 deletions). Full, engine, and pipe-only exports are checked; usertests is unchanged. The v0.0 full identifier 24d95707e371f5bba684c601f7841f7488a17ccc corrects an earlier short-hash transcription. Code validation/preservation is complete; report environment metadata and later user-directed report/GitHub/team work remain.
42. Captured environment-versions.txt and screenshot 43: Ubuntu 26.04.1 LTS, kernel 6.18.54.1-microsoft-standard-WSL2, Git 2.53.0, RISC-V GCC 15.2.0, QEMU 10.2.1, and GDB 17.1. The Linux kernel string confirms WSL2; the Windows WSL application package version is not claimed. Implementation, local checkpoint, and report evidence collection are complete; final document creation and later GitHub/team steps await the user's next instruction.

The user performed source edits, builds, tests, and Git commands. The assistant explained steps, inspected supplied evidence and the guide, and maintained these report records.

## Proposed report sections

### Project purpose and scope

Explain xv6 memory-management enhancements: lazy allocation, COW fork, memory statistics, and the intended swap/replacement work. State the implemented scope as of the report's compilation date.

### Environment and setup

Explain Windows, WSL/Ubuntu, QEMU, and xv6 as separate layers. Record tool versions, the project path, historical baseline choice, build settings, setup errors, and fixes. Distinguish the initial modern upstream from the historical implementation baseline.

### Baseline and lazy allocation

Compare eager sbrk with deferred physical allocation. Describe handler integration, valid-range checks, zeroing, mapping holes, and kernel/user copies. Use v0.1-summary.md and v0.1-lazy-allocation.patch.

### Copy on write

Explain page reference counts, writable-to-COW permission changes, parent/child sharing, write recovery, cleanup, and TLB flushing. Include the wait-order regression/fix. Use v0.2-summary.md and v0.2-cow.patch.

### Memory statistics

Describe the shared structure, syscall path, user command, and counter meanings. Counters are cumulative and include shell/command activity. Handler-call counts are not solely hardware traps. Distinguish actual COW copies from single-owner permission restoration. Use v0.3-summary.md.

### Swap and page replacement

Describe the initial measured swap implementation: tracking eligible private frames, FIFO/Clock selection, direct swap-block I/O, swapped-PTE restoration/cleanup, COW interaction, and buffer prefaulting. The first instructed FIFO demonstration completes a 12 MB workload with zero bad pages despite an 8 MB usable RAM limit, compared with termination before swap. Clearly distinguish that observed result from pending repeated-cleanup, COW/swap, Clock, and full regression validation. Configuration flags were instructed but not displayed in screenshot 28.

### Results and comparisons

| Experiment | Before | Recorded after result | Evidence |
| --- | --- | --- | --- |
| 200 MB heap reservation | Eager request failed | Lazy request succeeded | 03-eager-baseline-result.txt; 07-lazy-allocation-results.png |
| Sparse 256 MB test | Not measured | lazytest OK, 656 sparse-write-touched pages | 07-lazy-allocation-results.png |
| 80 MB touched before fork | Eager fork failed | COW test OK | 09-before-cow.png; 17-cow-sbrkfail-fix-targeted-pass.png |
| 50 forks with 20 MB touched | 38 ticks | Observed 3–4 ticks | 09-before-cow.png; 17-cow-sbrkfail-fix-targeted-pass.png |
| COW full regression | First run failed at sbrkfail | Full pass after wait fix | 15-v0.2-usertests-sbrkfail.txt; 18-v0.2-full-usertests-pass.png |
| Free pages across cowtest/lazytest | 32,500 initially | 32,500 after each test | 20-v0.3-memstat-results.png |
| Full v0.3 suite and free-page recovery | 32,500 in initial statistics | ALL TESTS PASSED; 32,500 free pages afterward | 21-v0.3-full-suite-and-memstat.png |
| Constrained-memory swap reference | No swap with 8 MB usable xv6 RAM | 4 MB control passes; 12 MB child exits -1; free pages recover to 1,870 | 25-no-swap-pressure-baseline.png |
| Initial after-swap pressure result | 12 MB child previously exits -1 | 3,072 pages checked, zero bad pages, child status 0; active swap I/O | 28-fifo-swap-memhog-results.png |
| Repeated after-swap pressure | Initial post-run free pages/used slots 1,857/one | Same counts after two additional successful 12 MB runs; no accumulation observed in this sequence | 29-fifo-repeat-pressure-results.png |
| COW plus swap interaction | New 6 MB parent/child workload | Child bad=0, parent OK, active COW copies and swapping; free pages/used slots 1,857/one | 30-fifo-swapfork-results.png |
| FIFO hotcold measured phase | Clock reference pending | FIFO first run: 8,006 swap ins and 8,007 swap outs during three passes after initialization | 31-fifo-hotcold-results.png |
| Initial FIFO versus Clock comparison | FIFO measured ins/outs 8,006/8,007 | Clock 7,691/7,684; 3.9% fewer ins and 4.0% fewer outs in these individual runs; no timing measured | 32-clock-hotcold-results-and-command-retry.png |
| Clock correctness sequence | Before sequence: free pages/used slots 1,857/one | memhog 4/12, swapfork, lazytest pass; same final resource readings | 33-clock-correctness-results.png |
| First full v0.4 regression | Expected final full-suite pass | Failed countfree write and reported 283-page deficit; diagnosis pending | 34-v0.4-usertests-countfree-failure.png |
| Focused test after kernel pipe repair | Original full suite had countfree write failure | Isolated pipe1 and driver countfree checks pass; full rerun still pending | 36-pipe-staging-focused-test-pass.png |
| Complete regression after pipe repair | Original full suite failed its final counter | Fresh full suite passes; free pages 32,306 to 32,307, used slots zero to one | 37-v0.4-fresh-boot-and-usertests-start.png; 38-v0.4-full-usertests-pass-and-memstat.png |
| Final integrated-kernel fork timing | Original eager fork benchmark 38 ticks | Six ticks after integration/pipe repair; earlier COW-only results 3–4 ticks retained separately | 40-v0.4-final-forkbench-and-resource-recovery.png |

Treat timings as individual observations, not controlled averages. The free-page snapshots show no net loss across the targeted workloads and full v0.3 suite; they do not measure peak RAM use or prove all paths leak-free. The full v0.3 suite and Git release checkpoint are complete. Initial swap workload results are recorded; repeated cleanup, COW/swap interaction, and FIFO/Clock comparisons remain pending.

### Troubleshooting history

Include the missing emulator package, path-space quoting error, historical boot compatibility issue, separate exec OK failed shell input, initially unexplained forkbench launch failure, and sbrkfail regression with its validated wait fix. Expected negative-test diagnostics followed by OK must be distinguished from actual FAILED suite results. Preserve original screenshots and do not retouch away errors.

### Reproducibility and version control

- Historical upstream starting revision: c4de4b0285e1c8e5348bdd31f597fe958f37b912.
- Local eager baseline: 24d95707e371f5bba684c601f7841f7488a17ccc, tag v0.0, branch v0.0-baseline; direct export corrects the earlier screenshot transcription.
- Lazy release: f41e29304c11e81b29ea870a0cb372f736ec1369, tag v0.1, branch v0.1-lazy.
- Comparison-program checkpoint: 28c7475.
- COW release: 87437a019947f47f8f811dde33b31757f01d9091, tag v0.2, branch v0.2-cow.
- Memory-statistics release: 712951e42df27676a9dd88fb76264b51402dc1cd, tag v0.3, branch v0.3-memstat.
- Memory/swap release: 93fa86df3ddda8691cc2c260b373dd58f726620f, tag v0.4, branch v0.4-swap; clean working tree confirmed.
- Current development branch: v0.4-swap.
- Source comparisons: v0.1-lazy-allocation.patch, v0.2-cow.patch, v0.3-memstat.patch, v0.4-swap.patch, v0.4-swap-engine.patch, and v0.4-pipe-fix.patch.

GitHub publication and splitting work with the other team member were explicitly deferred until the memory portion is finished. Do not claim a personal GitHub repository, push, PR, or collaborator invitation has been completed.

## Remaining work before final claims

- Use the preserved no-swap reference and validated local v0.4 source to write the measured comparisons. Retain the unchanged memhog wrapper, bounded resource-stability results, unidentified residual slot, guide design limits, and pre-pipe-repair policy-run context.
- Runtime checks, final demos, clean v0.4 checkpoint, and full reference/release IDs are recorded. Retain original failures, diagnosis/repairs/reruns, individual-run timing limits, and configuration-evidence limitations in the report.
- Use the captured environment snapshot (environment-versions.txt and screenshot 43); full baseline/release identifiers are verified. Do not infer an unmeasured Windows WSL package version or claim the QEMU package string changes Ubuntu's distribution identity.
- Preserve each new screenshot, failure, fix, configuration, and measurement as work continues.
- Compile the final report when requested, using the required institutional format if supplied.
