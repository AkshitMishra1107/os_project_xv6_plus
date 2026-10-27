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
| **v0.12** | `dbgmini` | Reason 1 (exec stop) → reason 3 (exited) | Command not found | *(Pending v0.12 run)* | `v012-dbgmini-session.txt` |
| **v0.13** | `dbg dbgtarget` | Breakpoints hit at `middle` & `inner`, symbolized `bt`, result `12` | Crashes with scause 3 | *(Pending v0.13 run)* | `v013-dbg-session.txt` |
| **v1.0** | `finaldemo` | 3 threads touched 1536 pages, signal 10 received | Fails (missing features) | *(Pending v1.0 run)* | `v10-finaldemo-session.txt` |

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

