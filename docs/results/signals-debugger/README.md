# Signals, Tracing, and Debugger Results

These observations come from saved project runs through the validated `v1.0` integrated release. Relevant screenshots and output logs are organized below.

---

## Before and After

| Capability | Earlier Baseline Behavior (v0.0) | Behavior After Feature Extension | Evidence Artifact |
| :--- | :--- | :--- | :--- |
| **System Call Tracing (`read`)** | `exec trace failed` (command absent; no observability) | Intercepts read calls with byte counts and return codes | [Trace Read](screenshots/01-trace-read-test.png), [Output](outputs/v010-trace-session.txt) |
| **Command Tracing (`exec`)** | System calls execute silently | Logs PID, syscall name, arguments, and return status | [Trace Exec](screenshots/02-trace-exec-test.png) |
| **Fork Trace Inheritance** | Child execution cannot be monitored | Children inherit parent's trace mask across `fork()` | [Trace Fork](screenshots/03-trace-fork-inheritance.png) |
| **Tracing Comparison** | Baseline lacks syscall 29 | Complete runtime syscall interception | [Trace Comparison](screenshots/04-baseline-vs-plus-trace.png) |
| **Memory Regression** | Baseline lacks memory extensions | `lazytest`, `cowtest`, `forkbench` pass cleanly | [Memory Regression](screenshots/05-regression-lazy-cow-forkbench.png) |
| **POSIX Signal Handling** | Only uncatchable `kill(pid)` exists | Full 16-signal framework, custom handlers, `sigreturn` | [Sigtest](screenshots/06-signals-sigtest.png), [Output](outputs/v011-signals-session.txt) |
| **Signals Comparison** | `exec sigtest failed` | Transparent register preservation and wakeups | [Signals Comparison](screenshots/07-signals-vs-baseline.png) |
| **Kernel Debugger Primitives** | No process introspection or control | 6 debugger syscalls: traceme, wait, cont, getregs, peek, poke | [Dbgmini](screenshots/08-debugger-kernel-dbgmini.png), [Output](outputs/v012-dbgmini-session.txt) |
| **Debugger Comparison** | Breakpoints kill process (cause 3 panic) | Non-fatal `ebreak` halts child and notifies parent | [Debugger Comparison](screenshots/09-debugger-vs-baseline.png) |
| **Interactive Debugger Tool** | No in-guest debugger tool | Interactive `dbg` CLI with breakpoints, step-over, symbols | [Dbg Tool Session](screenshots/10-dbg-tool-session.png), [Output](outputs/v013-dbg-session.txt) |
| **Symbolized Backtraces** | No call stack inspection | `bt` unwinds RISC-V frame pointers with ELF function symbols | [Symbolized Backtrace](screenshots/11-dbg-symbolized-backtrace.png) |
| **Final Integration Demo** | Subsystems untested in unison | Multi-threaded lazy memory stress with concurrent signals | [Final Integration](screenshots/12-final-integration-finaldemo.png), [Output](outputs/v10-finaldemo-session.txt) |
| **Whole System Regression** | Baseline lacks all features | 100% test pass rate across all 15 project milestones | [Full System Summary](screenshots/13-full-system-summary.png) |

---

## Reproduce the Experiments

From the Linux or WSL repository root directory, build and launch the integrated kernel:

```bash
make clean
make qemu
```

At the xv6 prompt (`$`), run each command to verify functionality:

```text
trace 32 cat README
trace 128 echo hi
sigtest
dbgmini
dbg dbgtarget
finaldemo
lazytest
cowtest
```

Exit QEMU at any time using **Ctrl+A, then X**.
