# xv6 Plus

An operating systems mini project extending MIT xv6 for RISC-V. The completed memory implementation is preserved at **tag `v0.4`**, commit `93fa86df3ddda8691cc2c260b373dd58f726620f`.

## Current implementation

| Area | Status |
| --- | --- |
| Lazy allocation and sparse address spaces | Implemented and tested |
| Copy on write fork and page reference counts | Implemented and tested |
| Memory statistics syscall and command | Implemented and tested |
| FIFO and Clock swap with low memory demonstrations | Implemented and tested with one CPU |
| Pipe copies outside the pipe spinlock | Implemented; full `usertests` passed afterward |
| Threads and synchronization | Planned next; Akshit owns v0.5 through v0.9 |
| Tracing, signals and debugger | Planned; teammate owns v0.10 through v0.13 |

The future milestones in the guides are implementation targets. They are not features already present in this checkout.

## Build and run

Use Ubuntu or another suitable Linux environment with Git, make, a RISC-V GCC/binutils toolchain, QEMU RISC-V, Perl and the other dependencies listed in the supplied guides. The tested owner environment is recorded in [environment versions](docs/memory/evidence/environment-versions.txt).

```bash
git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git xv6-plus
cd xv6-plus
make clean
make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1
```

At the xv6 shell run the commands individually:

```text
baseline
lazytest
cowtest
forkbench
memstat
usertests
```

Exit QEMU with **Ctrl A then X** before running Linux build or Git commands.

For reduced memory swap demonstrations:

```bash
make clean
make qemu CPUS=1 RAM_MB=8 SWAP_POLICY=1
```

Inside xv6 run `memstat`, `memhog 4`, `memhog 12`, `swapfork`, `hotcold` and another `memstat`. Repeat in a fresh build with `SWAP_POLICY=2` for Clock. Policy 1 is FIFO. `RAM_MB` controls usable xv6 RAM; QEMU remains provisioned with 128 MB. The image recipe appends the swap area automatically.

Always run `make clean` when switching CPU count, usable RAM or policy. Swapping is supported only with **`CPUS=1`**. The planned thread demonstrations use three CPUs and 128 MB; they require the later implementation.

## Project history and attribution

This repository preserves the original [MIT xv6 RISC-V](https://github.com/mit-pdos/xv6-riscv) history and license. GitHub can therefore show file dates from years ago and many inherited commits. Those dates describe earlier upstream changes. The project milestones below identify our additions; we do not claim authorship of the inherited teaching kernel.

| Checkpoint | Commit | Project change |
| --- | --- | --- |
| `v0.0` | `24d9570` | Verified eager allocation baseline |
| `v0.1` | `f41e293` | Lazy allocation |
| Comparison tests | `28c7475` | Fork comparison programs before COW |
| `v0.2` | `87437a0` | Copy on write and wait cleanup repair |
| `v0.3` | `712951e` | Memory statistics |
| `before-swap` | `c325f8f` | Constrained memory baseline without swap |
| `v0.4` | `93fa86d` | FIFO and Clock swap with safe pipe copies |

The branches `v0.0-baseline`, `v0.1-lazy`, `v0.2-cow`, `v0.3-memstat` and `v0.4-swap` preserve the earlier working checkpoints. A branch tip and its corresponding release tag can differ when comparison programs were added afterward.

To inspect our changes from the historical source ancestor:

```bash
git log --oneline c4de4b0..v0.4
git diff --stat c4de4b0 v0.4
git diff v0.0 v0.1
git diff v0.1 v0.2
git diff v0.3 v0.4
```

## Recorded memory observations

- Eager `sbrk` for 200 MB failed; the lazy version reserved it successfully. Sparse `lazytest` touched 656 pages successfully. Reservation size is not resident memory usage.
- The recorded 50 fork benchmark of a 20 MB process took 38 ticks before COW, 3 to 4 ticks in COW only runs, and 6 ticks in the final integrated v0.4 observation. These are distinct measurements and configurations.
- At 8 MB usable RAM, `memhog 12` failed without swap. With swap it verified 3072 pages with zero bad pages and child status 0, including repeated runs.
- The recorded hotcold measured phase showed FIFO 8006 swap ins and 8007 outs, versus Clock 7691 ins and 7684 outs. There was one recorded run per policy; these results predate the final pipe repair and are not runtime measurements.
- Complete `usertests` passed after the pipe staging repair. The test source was not weakened. A single residual used swap slot was stable across repeated observations; its owner was not traced, so the evidence does not establish universal absence of leaks.

See the [memory summary](docs/memory/evidence/v0.4-summary.md) and [progress record](docs/memory/evidence/progress-log.md) for configurations, errors, fixes, measurements and screenshots.

## Two person workflow

| Person | Branch | Responsibility |
| --- | --- | --- |
| Akshit | `feature/threads-sync` | Threads and synchronization |
| Teammate | `feature/signals-debugger` | Tracing, signals and debugger |
| Both | `main` through reviewed pull requests | Integration and releases |

Each person uses their own local clone, Git identity and GitHub account. Commit and push to your own feature branch, open a focused pull request to `main`, and have the other person review the shared interfaces and test evidence. Use **Create a merge commit** to preserve individual commits. Keep these long lived branches after milestone merges and merge updated `origin/main` into them. Akshit creates release tags only after the integrated version passes its checks.

The teammate can build an adapted tracing prototype now. Full Part 3 integration requires the v0.9 thread foundation. The detailed manuals explain this dependency and the reserved syscall registry.

## Guides and evidence

- [Akshit implementation manual](docs/guides/Akshit%20Threads%20and%20Synchronization%20Guide.docx) and [searchable text](docs/guides/Akshit%20Threads%20and%20Synchronization%20Guide.md).
- [Teammate implementation manual](docs/guides/Teammate%20Signals%20Tracing%20and%20Debugger%20Guide.docx) and [searchable text](docs/guides/Teammate%20Signals%20Tracing%20and%20Debugger%20Guide.md).
- [Documentation index](docs/README.md), including original guides and preserved memory evidence.
- [Threads records](docs/threads/README.md), [signals and debugger records](docs/signals-debugger/README.md) and [joint integration records](docs/integration/README.md).

The upstream [README](README) and [LICENSE](LICENSE) remain in place. No kernel feature or test was changed by the collaboration bootstrap.
