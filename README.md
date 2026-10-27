# xv6 Plus

We extended MIT's RISC-V teaching operating system with **lazy memory allocation, copy-on-write fork, memory statistics, and disk swapping with FIFO and Clock replacement**.

The latest validated memory release is [`v0.4`](https://github.com/AkshitMishra1107/os_project_xv6_plus/tree/v0.4) (`93fa86d`). Threads and synchronization are in development; tracing, signals and a debugger are the next team contribution.

## What we added

The baseline below is **our verified eager-allocation checkpoint `v0.0`**. It is not a claim about every version of upstream xv6.

| Feature | Before our change | What the extension enables |
| --- | --- | --- |
| Lazy allocation | A 200 MB eager allocation failed | Reserve the address range first; allocate and zero pages when accessed. Sparse test: 656 pages touched successfully. |
| Copy-on-write fork | Fork copied resident user pages; the large-process fork test failed | Share pages until a write requires a private copy. `cowtest` passes. |
| Memory statistics | Our baseline had no `memstat` interface | Inspect faults, lazy allocations, actual COW copies, free pages and swap activity. |
| Disk swapping | The 12 MB workload failed with 8 MB usable RAM | Verify all 3,072 workload pages with zero bad pages by moving pages between RAM and a 16 MB swap area. |
| FIFO and Clock | No page replacement in our baseline | Choose a replacement policy and compare its swap activity on the same workload. |

Read [how the features work and where the code is](docs/features/memory.md), then see the [before-and-after results, outputs and screenshots](docs/results/memory/README.md).

## Measured results

| Experiment | Recorded observation |
| --- | --- |
| 50 forks of a 20 MB process | 38 ticks before COW; 3–4 ticks in COW-only runs; 6 ticks in the final integrated v0.4 run. These are separate observations. |
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

## Repository layout

```text
kernel/                  Operating-system implementation
user/                    User commands and executable tests
mkfs/                    Filesystem image builder
docs/features/           Feature explanations, diagrams and code links
docs/results/memory/     Selected measurements, outputs and screenshots
CONTRIBUTING.md          Team branches, commits, tests and pull requests
```

Kernel and user source stay in their build-compatible locations. Large implementation manuals, editor screenshots, raw development logs and intermediate patches are retained locally for the report, rather than repeated in the public documentation.

## Contributions and next features

Akshit implemented the validated memory milestones and is developing threads and synchronization on `feature/threads-sync`. The teammate is starting tracing, signals and debugger work on `feature/signals-debugger`. Completed contributions will be linked to their commits, pull requests and test evidence. See [the team workflow](CONTRIBUTING.md) before the first teammate commit.

| Milestone | State |
| --- | --- |
| v0.1 lazy allocation through v0.4 memory and swap | Implemented and validated |
| v0.5 thread-ready memory through v0.9 synchronization demos | In development; later releases depend on earlier milestones |
| v0.10 tracing through v0.13 debugger tool | Planned; a tracing prototype may be developed before the thread integration is ready |

[Compare our memory changes](https://github.com/AkshitMishra1107/os_project_xv6_plus/compare/v0.0...v0.4). The original MIT history and [license](LICENSE) are preserved. The plain [upstream README](README) is also retained because the filesystem-image recipe uses it.
