# xv6 Plus

We extended MIT's RISC-V teaching operating system with **lazy memory allocation, copy-on-write fork, memory statistics, disk swapping with FIFO and Clock replacement, and shared-memory threads with futex-based synchronization**.

The tagged memory release is [`v0.4`](https://github.com/AkshitMishra1107/os_project_xv6_plus/tree/v0.4) (`93fa86d`). On this implementation branch, **v0.5–v0.9 code is implemented and tested, with review, main-branch integration and release tags pending**. [PR #3](https://github.com/AkshitMishra1107/os_project_xv6_plus/pull/3) contains the v0.5 prerequisite; `feature/threads-implementation` adds the later milestones. Nikunj’s tracing work stays separate in [draft PR #2](https://github.com/AkshitMishra1107/os_project_xv6_plus/pull/2).

## What we added

The baseline below is **our verified eager-allocation checkpoint `v0.0`**. It is not a claim about every version of upstream xv6.

| Feature | Before our change | What the extension enables |
| --- | --- | --- |
| Lazy allocation | A 200 MB eager allocation failed | Reserve the address range first; allocate and zero pages when accessed. Sparse test: 656 pages touched successfully. |
| Copy-on-write fork | Fork copied resident user pages; the large-process fork test failed | Share pages until a write requires a private copy. `cowtest` passes. |
| Memory statistics | Our baseline had no `memstat` interface | Inspect faults, lazy allocations, actual COW copies, free pages and swap activity. |
| Disk swapping | The 12 MB workload failed with 8 MB usable RAM | Verify all 3,072 workload pages with zero bad pages by moving pages between RAM and a 16 MB swap area. |
| FIFO and Clock | No page replacement in our baseline | Choose a replacement policy and compare its swap activity on the same workload. |
| Threads | Separate process address spaces | Up to eight members share a PID, heap and page table, with individual trapframes and stacks. |
| Futex and synchronization library | No corresponding API in our baseline | Contended mutexes sleep; condition variables coordinate predicates; semaphores limit concurrent users. |

Read [memory mechanisms and code](docs/features/memory.md) and [memory comparison evidence](docs/results/memory/README.md). The [thread architecture and synchronization diagrams](docs/features/threads.md) and [executed thread validation](docs/results/threads/README.md) explain the newer additions.

## Measured results

| Experiment | Recorded observation |
| --- | --- |
| 50 forks of a 20 MB process | 38 ticks before COW; 3â€“4 ticks in COW-only runs; 6 ticks in the final integrated v0.4 run. These are separate observations. |
| 12 MB pressure workload with 8 MB usable RAM | Failed without swap; passed with swap, including three repeated runs. |
| Final memory regression | Full `usertests`: `ALL TESTS PASSED` after the pipe-copy repair. |

These are recorded experiments, not general performance guarantees. [Configurations and limits](docs/results/memory/README.md#measurement-limits) accompany the results.

## Build and try the memory release

Use Ubuntu or WSL2 with Git, make, a RISC-V GCC/binutils toolchain, QEMU RISC-V and Perl.

```bash
git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git xv6-plus
cd xv6-plus
make clean
make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1
```

At the xv6 `$` prompt, run these individually:

```text
baseline
lazytest
cowtest
forkbench
memstat
usertests
```

Exit QEMU with **Ctrl+A, then X**. To try swapping, rebuild with `RAM_MB=8` and run `memhog 4`, `memhog 12`, `swapfork`, then `memstat`. Policy `1` is FIFO; `2` is Clock. Run `make clean` before changing configuration. Swapping is supported with **one CPU**. QEMU supplies 128 MB; `RAM_MB` limits the memory xv6 uses.

## Try the thread implementation

```bash
git fetch origin
git switch feature/threads-implementation
make clean
make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1
```

Run `threadtest`, `racetest`, `prodcons`, `philosophers`, `semtest`, `exittest`, `threadmem`, and `usertests` individually at the xv6 prompt. Three-CPU stress checks passed ten rounds and full usertests passed on one and three CPUs. One-CPU FIFO and Clock pressure tests also passed with four workers verifying a shared 12 MB region. [Exact configurations and recorded outputs](docs/results/threads/README.md) include timing limits and memory snapshots. Eviction is disabled in SMP builds; swapping remains supported with one CPU.

## Repository layout

```text
kernel/                  Operating-system implementation
user/                    User commands and executable tests
mkfs/                    Filesystem image builder
docs/features/           Feature explanations, diagrams and code links
docs/results/            Selected per-feature measurements, outputs and screenshots
CONTRIBUTING.md          Team branches, commits, tests and pull requests
```

Kernel and user source stay in their build-compatible locations. Large implementation manuals, editor screenshots, raw development logs and intermediate patches are retained locally for the report, rather than repeated in the public documentation.

The [v0.5 preparation](docs/features/thread-ready-memory.md) and its [candidate validation results](docs/results/thread-ready-memory/README.md) are documented separately from the earlier memory release.

## Contributions and next features

Akshit implemented the memory, thread and synchronization milestones. `feature/threads-sync` holds the v0.5 prerequisite; `feature/threads-implementation` preserves the tested v0.6–v0.9 commits while review proceeds. Nikunj Trehan (`nikunjtrehan`) has pushed a tracing prototype on `feature/signals-debugger`; signals and debugger work remain planned, and final tracing integration depends on the thread foundation. Completed contributions will be linked to their commits, pull requests and test evidence. See [the team workflow](CONTRIBUTING.md) before the first teammate commit.

| Milestone | State |
| --- | --- |
| v0.1 lazy allocation through v0.4 memory and swap | Implemented and validated |
| v0.5 thread-ready memory | Implemented and tested; PR #3 review, integration and tag pending |
| v0.6 through v0.9 threads and synchronization | Implemented, tested and committed on the implementation branch; review, integration and tag pending |
| v0.10 tracing through v0.13 debugger tool | Tracing prototype pushed in draft PR #2; thread integration, signals and debugger still pending |

[Compare our memory changes](https://github.com/AkshitMishra1107/os_project_xv6_plus/compare/v0.0...v0.4). The original MIT history and [license](LICENSE) are preserved. The plain [upstream README](README) is also retained because the filesystem-image recipe uses it.
