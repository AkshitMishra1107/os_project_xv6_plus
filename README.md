# xv6 Plus

RISC-V xv6 extended with lazy allocation, copy-on-write fork, disk paging, kernel threads, futex synchronization, syscall tracing, user signals and an in-guest debugger.

Start with the **[project report](docs/reports/xv6-plus-final-report.docx)**, [architecture and source map](docs/architecture.md), and **[current validation](docs/results/validation-upgrade/README.md)**. The report explains what changed, how it works, why each comparison matters and the observed costs and benefits.

## What changed

| Area | Historical eager reference | Integrated implementation |
|---|---|---|
| Heap | Back all positive growth immediately | Reserve first; zero and map on valid first access |
| Fork | Copy resident pages | COW sharing for single-member fork; private copies for multithreaded fork |
| Pressure | Allocation fails without backing | FIFO, Clock or seeded Random disk paging on one CPU |
| Diagnostics | No project memory interface | Seven counters plus bitmap/PTE swap-owner audit |
| Shared execution | Separate process mappings | Up to eight kernel-scheduled group members with shared mappings |
| Coordination | No project thread library | Futex-backed mutexes, conditions and semaphores |
| Observation | No selective trace command | Masked syscall results with thread inheritance |
| Control | Termination through kill | User handlers, pending signals, pause and restoration |
| Debugging | External tools only | Opted-in child stops, inspection, breakpoints and symbolized frames |

The reference is project v0.0 on historical MIT code. The initially cloned newer upstream already has opt-in `sbrklazy`; ordinary `sbrk` is eager there. [Versions](docs/versions.md) identify the compared checkpoints. Standard algorithms and MIT-lab correspondence are attributed in [references](docs/references.md).

## Build and try it

```bash
git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git
cd os_project_xv6_plus
make clean
make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1
```

Run `lazytest`, `cowtest`, `racetest`, `prodcons`, `sigtest`, `dbgmini` and `finaldemo` at the xv6 prompt. For interactive inspection use `dbg dbgtarget`, then `break inner`, `run`, `bt`, `regs` and `continue`. Exit QEMU with Ctrl+A then X.

For pressure, rebuild clean with `CPUS=1 RAM_MB=8 SWAP_POLICY=1`; run `memhog 12`, `swapfork`, `threadswap`, `swapcheck` and `finaldemo 4`. Policy 2 is Clock and 3 is seeded Random. QEMU exposes 128 MiB; RAM_MB limits the managed range, including kernel overhead. One page is 4096 bytes and the swap area is 16 MiB.

## Checked results

- All four full `usertests` runs pass: one-CPU FIFO, Clock and Random with nonzero paging activity, and three-CPU SMP.
- A 200 MiB ordinary reservation changes from failure to success; the sparse test touches 656 pages. The 80 MiB COW test checks fork capacity and private writes.
- The 12 MiB pressure workload checks every byte of all 3072 pages under an 8 MiB managed limit. The four-block corruption negative control detects 4/4 corruptions.
- Swap audits map the remaining two slots to live `init` pages and report zero orphan, missing, duplicate or invalid references.
- Controlled unprotected updates produce 1000 instead of 4000; mutex protection produces 4000.
- The combined one-CPU test runs three actual workers, 12 MiB of full-page checked data, synchronization and a signal while paging is active.

The [current comparison tables](docs/results/validation-upgrade/README.md) include 60 fresh-guest paging cases across four access patterns, 20 same-source COW on/off cases including write-all children, and 15 equal-work parallel cases. Raw console bytes, native QEMU captures, all samples and trace/model checks are organized together. [Reproduce the tests](tools/README.md).

**Configuration:** eviction and swap audit run on one CPU. Three-CPU scaling has eviction disabled. Multithreaded fork copies privately. Signals and the debugger implement the compact interfaces documented in the report.

## Find things

```text
kernel/                 Operating-system mechanisms
user/                   Commands, libraries and checked workloads
tools/                  Reproducible validation and independent models
docs/architecture.md    Architecture and complete source-purpose map
docs/features/          Mechanism explanations
docs/results/           Current validation and historical evidence
docs/reports/           Combined project report
docs/references.md      MIT labs, papers and specifications
```

`main` contains the complete project; tags preserve earlier versions. Use a temporary branch while developing, merge reviewed changes and remove that branch afterwards. See [CONTRIBUTING.md](CONTRIBUTING.md).

Project team: Akshit Mishra, Nikunj Trehan and Pratyaksh Rana. MIT supplies the original xv6 foundation; its [license](LICENSE) and [upstream README](README) remain intact.
