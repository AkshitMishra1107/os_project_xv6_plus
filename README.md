# xv6 Plus

An educational extension of MIT's RISC-V teaching operating system with **lazy allocation, copy-on-write fork, memory statistics, FIFO/Clock disk swapping, and threads with futex-based synchronization**.

The memory milestones and v0.5–v0.9 thread foundation are integrated into `main`. The historical memory-only release is [`v0.4`](https://github.com/AkshitMishra1107/os_project_xv6_plus/tree/v0.4); the integrated foundation is [`v0.9`](https://github.com/AkshitMishra1107/os_project_xv6_plus/tree/v0.9). Nikunj's tracing work remains separate in [draft PR #2](https://github.com/AkshitMishra1107/os_project_xv6_plus/pull/2); signals and debugger completion are not claimed here.

## Start here

- [Memory implementation walkthrough](docs/presentation/memory-implementation-walkthrough.docx): six pages explaining the kernel changes, before-and-after behavior, code excerpts and observed results.
- [Detailed memory report draft](docs/reports/memory-management-draft.docx): extended implementation detail, original evidence, references and viva preparation.
- [What to present to the teacher](docs/presentation/memory.md): commands, evidence checklist and explanations.
- [MIT labs and other primary references](docs/references.md): direct lab links and how they relate to this project.
- [Memory mechanisms and source map](docs/features/memory.md) · [memory comparisons and original screenshots](docs/results/memory/README.md).
- [Thread architecture and synchronization](docs/features/threads.md) · [executed thread validation](docs/results/threads/README.md).

## What changed

The comparison baseline is **our eager checkpoint `v0.0`**, based on historical upstream `c4de4b0`. Newer upstream xv6 already contains lazy allocation. These are educational implementation and integration results, not claims of new algorithms or an MIT lab grading pass.

| Feature | Earlier project behavior | Added functionality |
|---|---|---|
| Lazy allocation | A 200 MiB eager request failed | Reserve virtual space first; allocate zeroed pages when used. The sparse test reports 656 write-touched pages. |
| Copy-on-write fork | Fork copied resident data; the 80 MiB fork test failed | Share protected frames until private writes are needed; the custom isolation test passes. |
| Memory statistics | No project memstat API | Observe handler calls, allocations, COW copies, free pages and swap activity. |
| FIFO and Clock swap | A 12 MiB workload failed under an 8 MiB usable-RAM limit | Check all 3072 pages with zero bad pages using a fixed 16 MiB disk area. |
| Threads | Separate process address spaces | Up to eight members share a PID, page table and heap, with separate trapframes and stacks. |
| Synchronization | No corresponding project API | Futex-backed mutexes, condition variables and semaphores coordinate shared work. |

## Recorded results

| Experiment | Observed result |
|---|---|
| 50 fork/wait cycles with 20 MiB touched | 38 ticks before COW; 3–4 in separate COW-only observations; 6 in the historical integrated memory run. These are not controlled averages. |
| 12 MiB pressure with 8 MiB usable xv6 RAM | Failed before swap; passed with swap and repeated correctness checks. |
| Full regression on later integrated source | usertests passed with one and three CPUs. |
| Thread stress | Ten SMP rounds passed; free pages stayed at 32300 and slots at zero after every round. |
| Shared-thread swap pressure | Four workers checked 12 MiB with zero bad pages in ten FIFO and three Clock runs. |

Historical measurements, exact configurations, source hashes and limits are linked from the result pages. Remaining occupied swap slots were stable in the recorded pressure sequences; their owners have not been traced. The timing results do not establish proportional speedup or a universal policy winner.

## Build and try

Use Ubuntu/WSL2 with Git, make, RISC-V GCC/binutils, QEMU RISC-V and Perl.

```bash
git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git xv6-plus
cd xv6-plus
make clean
make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1
```

At the xv6 prompt, run `memstat`, `baseline`, `lazytest`, `cowtest`, `forkbench` and `usertests` individually. Exit QEMU with Ctrl+A then X.

For swap pressure, rebuild clean with `CPUS=1 RAM_MB=8 SWAP_POLICY=1`, then run `memhog 4`, `memhog 12`, `swapfork`, `threadswap` and `memstat`. Policy 1 is FIFO; 2 is Clock. QEMU is provisioned with 128 MiB; RAM_MB limits the memory managed by xv6. **Swapping is supported with one CPU.**

For parallel thread demonstrations, rebuild clean with `CPUS=3 RAM_MB=128 SWAP_POLICY=1`. Run `threadtest`, `racetest`, `prodcons`, `philosophers`, `semtest`, `exittest`, and `threadmem`. Eviction is disabled in SMP builds. Always clean before changing CPU count, RAM limit or policy.

## Repository layout

```text
kernel/               Operating-system implementation
user/                 Commands, library and executable tests
mkfs/                 Filesystem image builder
docs/features/        Mechanisms, diagrams and source links
docs/results/         Selected outputs, screenshots and validation records
docs/reports/         Teacher-facing draft report
docs/presentation/    Demonstration and evidence guide
docs/references.md    Primary references and attribution
CONTRIBUTING.md       Team integration and contribution workflow
```

Source stays in its build-compatible locations. Large manuals, editor recovery files, raw repeated logs and intermediate patches are preserved in the local report archive instead of the public presentation.

## Attribution and progress

MIT provides xv6 and its original history; the [license](LICENSE) and plain [upstream README](README) are retained. Lazy allocation and COW correspond to established MIT labs. Our memstat interface and combined swap/replacement experiments extend the project beyond those individual exercises; see the [reference map](docs/references.md).

Akshit completed the memory and thread/synchronization foundation. [PR #3](https://github.com/AkshitMishra1107/os_project_xv6_plus/pull/3) and [PR #4](https://github.com/AkshitMishra1107/os_project_xv6_plus/pull/4) are merged with their commits preserved. Nikunj continues on `feature/signals-debugger`. Read [the team workflow](CONTRIBUTING.md) before integrating shared kernel changes.

[Compare historical memory work](https://github.com/AkshitMishra1107/os_project_xv6_plus/compare/v0.0...v0.4) · [Integrated validation](docs/results/threads/integration.md)
