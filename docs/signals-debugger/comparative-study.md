# Comparative Study: Stock xv6 vs. xv6-Plus (Signals, Tracing & Debugger)

**Author:** Teammate (`nikunjtrehan`)  
**Project:** xv6-Plus Extended Operating System Kernel  
**Base:** stock xv6 / milestone v0.4 (93fa86d) → Target: v1.0  
**Environment:** Ubuntu 24.04.3 LTS (WSL2 6.6.87.2), RISC-V GCC 13.3.0, QEMU 8.2.2  

---

## 1. Executive Summary

This study documents the systematic comparative evaluation between **Stock MIT xv6 (RISC-V)** and **xv6-Plus**, focusing on the capabilities introduced in **Part 3**:
1. **Dynamic System Call Tracing (`trace`)**
2. **POSIX-style Inter-Process Signals & User Handlers (`sigaction`, `sigsend`, `sigreturn`, `pause`)**
3. **In-Kernel Debugger Infrastructure & Interactive Debugger Tool (`dbg`, `copyout_force`, breakpoint traps, symbolized backtraces)**

Stock xv6 provides a clean educational UNIX-like kernel, but lacks observability, runtime introspection, asynchronous process signaling, and debugging primitives. This study benchmarks and contrasts baseline behavior against each milestone improvement.

---

## 2. Feature Comparison Matrix

| Capability | Stock MIT xv6 (v0.0) | xv6-Plus (After Part 3) | Improvement / Impact |
| :--- | :--- | :--- | :--- |
| **System Call Tracing** | ❌ None. System calls execute silently. No userland observability. | ✅ `trace <mask> <cmd>`. Traces calls 1..30 with return values. Inherited across `fork()` and preserved across `exec()`. | Enables Linux `strace`-like runtime observability of any program without recompilation. |
| **Signal Handling** | ❌ None. Only `kill(pid)` exists, which unconditionally kills the target process. | ✅ Asynchronous signal delivery (SIGINT, SIGTERM, SIGUSR1/2, SIGALRM, etc.). Custom handlers registered via `sigaction` / `signal()`. | Processes can intercept signals, run user-defined handlers, recover execution state via `sigreturn`, or ignore signals (`SIG_IGN`). |
| **Signal Interruption** | ❌ None. Sleeping processes cannot be interrupted except by being terminated. | ✅ `sleep()` returns `-1` early if a signal is pending. `pause()` sleeps until a signal arrives and wakes up immediately. | Allows responsive event-driven architectures and timeouts. |
| **Register & Context Recovery** | ❌ None. No mechanism to save/restore registers across asynchronous handler invocation. | ✅ Full `struct trapframe` saved into `sig_saved` and restored by `sigreturn()`, preserving `a0` return code and register integrity. | Complete transparency: user code state is preserved perfectly before and after handler execution. |
| **Breakpoint Traps** | ❌ Cause 3 (`ebreak`) causes unexpected kernel trap panic or process death. | ✅ Cause 3 intercepted in `usertrap()`. Traced child halts cleanly in `DBG_BREAK` and notifies parent debugger. | Enables non-fatal breakpoint debugging. |
| **Code Modification (`poke`)** | ❌ Read-only pages reject `copyout()`. Breakpoint insertion is impossible. | ✅ `copyout_force()` pins user pages and writes through read-only text pages with `fence.i` instruction cache flushes. | Debugger can inject 4-byte `ebreak` and 2-byte `c.ebreak` instructions into live binaries. |
| **Interactive Debugger** | ❌ None. Debugging requires host `gdb-multiarch` over QEMU GDB stub. | ✅ Native userland debugger `dbg` with interactive prompt, breakpoint step-over, register inspection (`regs`), and symbols. | Independent in-guest debugging capability without external host tools. |
| **Call Stack Backtrace** | ❌ None in userland. | ✅ Symbolic backtrace (`bt`) walking the RISC-V frame pointer chain (`fp - 8`, `fp - 16`) mapped to ELF symbol table function names. | Instant symbolized call hierarchy inspection at breakpoints. |

---

## 3. Subsystem Comparative Studies

### 3.1 Subsystem 1: System Call Tracing (`trace`)

- **Stock xv6 Baseline:**
  - Running `trace 32 cat README` outputs: `exec trace failed` (command does not exist).
  - Even if implemented in userland, syscall 29 returns `-1` with `unknown sys call 29`.
  - Kernel provides 0 visibility into system call execution, arguments, or return codes.
- **xv6-Plus Improvement:**
  - Syscall 29 (`SYS_trace`) configures bitmask on `p->trace_mask`.
  - Child processes inherit `trace_mask` during `fork()`.
  - Traced processes log each invoked syscall name and its return value directly:
    ```
    4: syscall read -> 1023
    4: syscall read -> 961
    4: syscall read -> 0
    ```

### 3.2 Subsystem 2: Inter-Process Signals

- **Stock xv6 Baseline:**
  - Only `kill(pid)` exists. If process 5 receives `kill(5)`, it is marked `killed = 1` and exits on next trap.
  - No signal numbers (cannot distinguish SIGUSR1 vs SIGTERM vs SIGKILL).
  - No custom signal handlers.
  - No `pause()` syscall.
- **xv6-Plus Improvement:**
  - 16 signals supported (`NSIG = 16`).
  - `sigaction` registers user handler function and `sig_restorer` stub.
  - On kernel return to user mode (`usertrap`), `deliver_signals()` checks `sig_pending`, saves the full user trapframe, points `epc` to handler, `ra` to restorer, and sets `a0 = sig`.
  - Restorer invokes `sigreturn` which restores all registers without clobbering return value.
  - `sleep()` is interruptible: exits immediately on signal with return `-1`.

### 3.3 Subsystem 3: Native Kernel Debugger & `dbg` Tool

- **Stock xv6 Baseline:**
  - If a user program executes `asm volatile("ebreak")`:
    Kernel `usertrap()` reports: `usertrap(): unexpected scause 0x0000000000000003 pid=... sepc=...` and kills the process.
  - Text segment pages are mapped strictly with PTE permissions `PTE_R | PTE_X` (no `PTE_W`). Any write to code triggers a page fault exception.
- **xv6-Plus Improvement:**
  - Child calls `dbg_traceme()`.
  - Kernel pauses child right after `exec()` with `DBG_EXEC`, allowing debugger to read ELF symbols and insert breakpoints.
  - Breakpoint execution (`ebreak` / `c.ebreak`) is caught by `usertrap()` and transitions child to `DBG_BREAK`.
  - Parent uses `copyout_force()` to swap original instruction bytes and step over breakpoints.
  - Call stack backtrace unwinds the frame pointer chain and matches PC to symbol names in `.symtab`.

---

## 4. Test Verification & Evidence Log

| Milestone | Command / Benchmark | Expected Output | Stock xv6 Baseline Result | xv6-Plus Result | Evidence Artifact |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **v0.10** | `trace 32 cat README` | Syscall `read` entries with byte counts, ending with `-> 0` | `exec trace failed` | ✅ **PASS:** Intercepted all `read` calls (512, 512, 512, 512, 244, 0 bytes) | `v010-trace-session.txt` & `01-trace-read-test.png` |
| **v0.10** | `trace 128 echo hi` | `syscall exec -> 2` printed, then output | `exec trace failed` | ✅ **PASS:** Printed `4: syscall exec -> 2` and `hi` | `v010-trace-session.txt` & `02-trace-exec-test.png` |
| **v0.10** | `trace 2 sh` → `ls` | `syscall fork -> <pid>` printed | `exec trace failed` | ✅ **PASS:** Fork inheritance verified, printed `5: syscall fork -> 6` | `v010-trace-session.txt` & `03-trace-fork-inheritance.png` |
| **v0.10** | Baseline Comparison | Side-by-side run of `trace 32 cat README` | `exec trace failed` | Full dynamic syscall tracing | `04-baseline-vs-plus-trace.png` |
| **v0.10** | Memory Regression | `lazytest`, `cowtest`, `forkbench` pass | N/A (baseline xv6 lacks memory extensions) | ✅ **PASS:** 656 lazy pages, COW OK, 6 ticks forkbench | `05-regression-lazy-cow-forkbench.png` |
| **v0.11** | `sigtest` (5 subtests) | `1 ok`, `2 ok`, `3 ok`, `4 ok`, `5 ok`, `ALL OK` | `exec sigtest failed` | ✅ **PASS:** Verified signal to self, inter-process signal waking `pause()`, default termination, ignored signal (`SIG_IGN`), and sleep interruption | `v011-signals-session.txt` & `06-signals-sigtest.png` |
| **v0.11** | Baseline Comparison | Side-by-side run of signal operations | `exec sigtest failed` (no signal infrastructure) | Complete POSIX signal model with register-safe restorer | `07-signals-vs-baseline.png` |
| **v0.12** | `dbgmini` | Reason 1 (exec stop) → reason 3 (exited) | `exec dbgmini failed` | ✅ **PASS:** Verified `dbg_traceme()`, `dbg_wait()` (reason 1 DBG_EXEC), `dbg_getregs()`, `dbg_peek()`, and `dbg_cont()` (reason 3 DBG_EXITED) | `v012-dbgmini-session.txt` & `08-debugger-kernel-dbgmini.png` |
| **v0.12** | Baseline Comparison | In-kernel process inspection & control primitives | `exec dbgmini failed` (no debugger syscalls) | Full kernel debugger foundation (`copyout_force`, `dbg_stop`, cause 3 `ebreak`) | `09-debugger-vs-baseline.png` |
| **v0.13** | `dbg dbgtarget` | Breakpoints hit at `middle` & `inner`, symbolized `bt`, result `12` | `exec dbg failed` | ✅ **PASS:** Full interactive debugging: breakpoints hit in sequence (`middle`, `inner`), symbolized frame unwinding (`bt`), step-over breakpoint restoration, output `outer(5) = 12` | `v013-dbg-session.txt` & `10-dbg-tool-session.png` |
| **v0.13** | Symbolized Backtraces | Call stack unwinding with ELF symbol translation | Crashes with scause 3 (no frame walking) | Stack unwinding across active call hierarchy (`#0` through `#5`) | `11-dbg-symbolized-backtrace.png` |
| **v1.0** | `finaldemo` | 3 threads touched 1536 pages, signal 10 received | Fails (missing features) | ✅ **PASS:** Multi-threaded stress touched 1536 pages, signal 10 delivered asynchronously, 1536 lazy page faults handled seamlessly | `v10-finaldemo-session.txt` & `12-final-integration-finaldemo.png` |
| **v1.0** | Whole System Regression | All suites pass (`lazytest`, `cowtest`, `forkbench`, `sigtest`, `dbgmini`, `dbg`, `finaldemo`) | Baseline lacks features | ✅ **PASS:** 100% test pass rate across all Part 1, Part 2, and Part 3 integrated subsystems | `13-full-system-summary.png` |

*(This log is updated dynamically as each milestone is executed and verified.)*

---

## 5. Visual Evidence Gallery (Milestone v0.10)

### 5.1 Dynamic System Call Tracing (`trace 32 cat README`)
![Trace Read Test](evidence/01-trace-read-test.png)

### 5.2 Command Execution Tracing (`trace 128 echo hi`)
![Trace Exec Test](evidence/02-trace-exec-test.png)

### 5.3 Trace Mask Inheritance Across `fork()` into Subshell
![Trace Fork Inheritance](evidence/03-trace-fork-inheritance.png)

### 5.4 Comparative Study: Stock MIT xv6 Baseline vs. xv6-Plus
![Baseline vs xv6-Plus](evidence/04-baseline-vs-plus-trace.png)

### 5.5 Regression Verification (Part 1 Compatibility Maintained)
![Memory Regression Tests](evidence/05-regression-lazy-cow-forkbench.png)

---

## 6. Visual Evidence Gallery (Milestone v0.11 — Signals)

### 6.1 Complete Signal Subsystem Verification (`sigtest`)
All 5 comprehensive test cases passing consistently across multiple runs, verifying:
1. Signal delivery to self and user-defined handler invocation with stack integrity
2. Inter-process signaling waking `pause()`
3. Default termination for uncaught signals (`SIG_DFL`)
4. Signal suppression for ignored signals (`SIG_IGN`)
5. Early wakeup and `-1` return value preservation on interrupted `sleep()` calls
![Signals Sigtest Verification](evidence/06-signals-sigtest.png)

### 6.2 Architectural Comparison: Signals in Stock xv6 vs. xv6-Plus
Side-by-side terminal session highlighting the fundamental architectural gap between stock MIT xv6 (which lacks any asynchronous signal infrastructure or user handlers) and xv6-Plus's complete POSIX signal framework.
![Signals Baseline vs xv6-Plus](evidence/07-signals-vs-baseline.png)

---

## 7. Visual Evidence Gallery (Milestone v0.12 — Debugger Kernel Support)

### 7.1 Debugger Kernel Primitives Verification (`dbgmini`)
Verification of the 6 fundamental in-kernel debugger primitives:
1. `dbg_traceme()`: Child process opts in to parent supervision.
2. `dbg_wait()`: Parent sleeps on `wait_lock` until child halts at `DBG_EXEC` (reason 1).
3. `dbg_getregs()`: Extract child's registers (`pc`, `sp`, `fp`, `a0`–`a7`).
4. `dbg_peek()`: Read instruction bytes directly at target `pc`.
5. `dbg_cont()`: Resume child execution and verify child completes and outputs.
6. `DBG_EXITED` (reason 3): Parent notified of target exit.
![Debugger Kernel Dbgmini Verification](evidence/08-debugger-kernel-dbgmini.png)

### 7.2 Architectural Comparison: Debugger Primitives in Stock xv6 vs. xv6-Plus
Stock xv6 has zero support for process introspection or control: attempting to run `dbgmini` fails with command missing, text pages are write-protected against breakpoint insertion, and breakpoint instructions (`ebreak`) kill processes unhandled. xv6-Plus provides full in-kernel debugger support with `copyout_force()` and non-fatal cause 3 handling.
![Debugger Baseline vs xv6-Plus](evidence/09-debugger-vs-baseline.png)

---

## 8. Visual Evidence Gallery (Milestone v0.13 — The `dbg` Tool)

### 8.1 Interactive Debugger Tool Workflow (`dbg dbgtarget`)
Full interactive debugging session demonstrating:
1. Dynamic ELF `.symtab` symbol loading for symbol-to-address resolution.
2. Software breakpoint injection (`break inner`, `break middle`) using RISC-V 32-bit `ebreak` and 16-bit compressed `c.ebreak`.
3. Program execution interception at `middle` function entry.
4. Single-step breakpoint bypass: restoration of original instruction bytes, temporary step breakpoint injection, and re-arming of original breakpoint.
5. Clean execution to program completion yielding verified calculation result: `dbgtarget: outer(5) = 12`.
![Debugger Tool Session](evidence/10-dbg-tool-session.png)

### 8.2 Symbolized Call Stack Backtrace (`bt`)
Verification of call stack unwinding via RISC-V frame pointers (`fp - 8` return address, `fp - 16` caller frame pointer):
1. Frame `#0`: Active function (`middle + 0` / `inner + 0`)
2. Frame `#1`: Caller function (`outer + 12` / `middle + 12`)
3. Frame `#2`: Main function (`main + 14` / `outer + 12`)
4. Frame `#3`: Entry wrapper (`start + 12`)
5. Frame `#4`–`#5`: Runtime allocator frames
![Symbolized Backtrace](evidence/11-dbg-symbolized-backtrace.png)

---

## 9. Visual Evidence Gallery (Milestone v1.0 — Final Integration & Verification)

### 9.1 Final Multi-Subsystem Integration Demo (`finaldemo`)
Simultaneous end-to-end stress test combining Part 1 (Lazy Page Allocation), Part 2 (Kernel Threading & Synchronization Mutexes), and Part 3 (Asynchronous POSIX Signals):
- 3 threads concurrently touch a total of 1,536 lazily allocated virtual memory pages.
- Asynchronous signal 10 (`SIGUSR1`) is generated, delivered, and handled across execution boundaries without corruption.
- Verified kernel memory counters report 1,536 lazy allocations and 1,538 total demand-paging page faults.
![Final Integration Demo](evidence/12-final-integration-finaldemo.png)

### 9.2 Complete System Regression & Verification Suite Summary
Full automated regression pass confirming zero regressions across all 15 project milestones:
1. Lazy Page Allocation (`lazytest`): All tests pass
2. Copy-on-Write Fork (`cowtest`): All tests pass
3. Fork Throughput Benchmark (`forkbench`): 6 ticks
4. Dynamic System Call Tracing (`trace`): All filters and fork inheritance verified
5. POSIX Signal Infrastructure (`sigtest`): All 5 subtests pass
6. In-Kernel Debugger Primitives (`dbgmini`): All 6 lifecycle states verified
7. Native Debugger CLI Tool (`dbg`): Breakpoints, symbols, step-over, backtraces verified
8. Multi-Subsystem Integration (`finaldemo`): All subsystems operate seamlessly in unison
![Full System Summary](evidence/13-full-system-summary.png)
