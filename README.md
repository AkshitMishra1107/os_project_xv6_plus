# xv6 Plus

RISC-V xv6 extended with lazy allocation, copy-on-write fork, disk swapping, kernel threads, futex synchronization, syscall tracing, user signals and an in-guest debugger.

**Start with the [final project report](docs/reports/xv6-plus-final-report.docx), [architecture and source map](docs/architecture.md), and [final validation](docs/results/final-integration/README.md).** The 30-page report focuses on each feature: original behavior, changed functions and modules, implementation mechanism, observed benefit and comparison evidence. It includes selected screenshots, outputs, tests and academic references without full C-file listings.

## What the project adds

| Area | Historical eager baseline | xv6 Plus |
|---|---|---|
| Allocation | Back positive heap growth immediately | Reserve addresses; zero and map pages on first use |
| Process creation | Copy resident pages at fork | Share COW frames for single-member fork; copy on private write |
| Memory pressure | No disk-backed paging | FIFO/Clock swapping with a fixed 16 MiB backing area |
| Observation | No project memory API | Seven-field `memstat` command and syscall |
| Shared execution | Separate process address spaces | Up to eight kernel-scheduled members share a PID, heap and page table |
| Coordination | No project thread library | Futex-backed mutexes, conditions and semaphores |
| Syscalls | No selectable trace interface | Masked PID/name/return tracing, including thread inheritance |
| Asynchronous control | Uncatchable `kill` only | User handlers, pending signals, ignore/default action and restoration |
| Debugging | No in-guest debugger | Child exec/breakpoint stops, memory/register inspection and symbolized frames |

The reference is **project v0.0**, based on historical MIT revision `c4de4b0`. Current upstream xv6 differs and already includes lazy allocation. This educational project does not claim new algorithms, POSIX compliance, or a pass from MIT's course grader.

## Build and demonstrate

Use Ubuntu/WSL2 with RISC-V GCC/binutils, QEMU RISC-V, make, Git and Perl.

```bash
git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git
cd os_project_xv6_plus
make clean
make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1
```

At the xv6 prompt try `lazytest`, `cowtest`, `racetest`, `prodcons`, `trace 32 cat README`, `sigtest`, `dbgmini` and `finaldemo`. For interactive debugging use `dbg dbgtarget`, then `break inner`, `run`, `bt`, `regs`, `continue`, `quit`. Run `usertests` for the inherited regression suite. Exit QEMU with Ctrl+A, then X.

**Paging pressure:** rebuild clean with `CPUS=1 RAM_MB=8 SWAP_POLICY=1`, then run `memhog 4`, `memhog 12`, `swapfork`, `threadswap`, `hotcold` and `memstat`. Policy 2 selects Clock. QEMU exposes 128 MiB; RAM_MB limits the range managed by xv6. Always clean when changing CPU/RAM/policy definitions.

## Evidence and measured improvements

- A historical 200 MiB eager request failed; lazy reservation succeeded. Reservation does not mean all 200 MiB are resident.
- The 80 MiB custom COW test changed from fork failure to successful isolation.
- Historical 50-cycle fork loops observed 38 ticks before COW, 3–4 in COW-only runs and 6 in a later memory integration. These are separate observations, not controlled averages.
- A checked 12 MiB workload failed under an 8 MiB limit before swap and completed with zero mismatches after swap.
- Thread tests verify protected counter 200000, queue total 10100, two-permit bounds, lifecycle and shared-memory checks.
- Fresh [combined validation](docs/results/final-integration/README.md) checks the integrated system and its debugger lifecycle repairs. Historical subsystem evidence stays in [memory results](docs/results/memory/README.md), [thread results](docs/results/threads/README.md) and [signals/debugger results](docs/results/signals-debugger/README.md).

**Boundaries:** eviction is one-CPU only; SMP builds do not swap. Signals are a compact Unix-inspired subset. Debugging controls a direct opted-in single-context child. Remaining occupied slots in historical pressure runs were not audited to owners; no universal leak-free or performance claim is made.

## Where things are

```text
kernel/                 OS mechanisms and inherited kernel code
user/                   Commands, libraries, workloads and tests
mkfs/                   Inherited filesystem image builder
docs/architecture.md    Architecture, interfaces and file-purpose inventory
docs/features/          Focused mechanism explanations
docs/results/           Commands, actual outputs and selected original captures
docs/reports/           Final full-project Word report
docs/presentation/      Short memory walkthrough and demonstration guide
docs/references.md      MIT assignments, specifications and research context
CONTRIBUTING.md         Simple main/temporary-branch/version workflow
```

## Versions and project team

**`main` is the complete integrated project. Tags preserve versions; old branches are not needed to view progress.** [Version guide](docs/versions.md) explains the historical checkpoints. Future work can use a short-lived branch and pull request, deleted after merge. The existing commits and MIT history remain intact.

Project team: [Akshit Mishra](https://github.com/AkshitMishra1107) and [Nikunj Trehan](https://github.com/nikunjtrehan). MIT supplies the original xv6 foundation; its [license](LICENSE) and [upstream README](README) are retained. Academic correspondence and project-specific differences are documented in [references](docs/references.md).
