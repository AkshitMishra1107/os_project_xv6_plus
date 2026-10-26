
# Teammate Signals Tracing and Debugger Implementation Guide

Two person xv6 Plus project   27 October 2026

This is the working manual for Teammate. Follow the shared GitHub routine first, then the complete implementation steps and source listings that follow. Akshit owns threads and synchronization. The teammate owns tracing, signals and the debugger. Both review shared kernel changes and validate the final integration.

The implementation chapters describe work to be performed and tests to run. Only the memory base v0.4 has completed implementation evidence. Future versions and their example output are acceptance targets, not results already obtained.

## Repository and saved starting point

https://github.com/AkshitMishra1107/os_project_xv6_plus

The memory milestone is tag v0.4 at 93fa86df3ddda8691cc2c260b373dd58f726620f. Its existing history and earlier milestone tags are preserved. main adds collaboration documentation on top of this source. Keep the v0.4 tag fixed so memory comparisons remain reproducible.

| Person | Working branch | Primary responsibility |
| --- | --- | --- |
| Akshit | feature/threads-sync | v0.5 through v0.9 and review of Part 3 |
| Teammate | feature/signals-debugger | Tracing prototype now then v0.10 through v0.13 |
| Both | main through reviewed PRs | Integrated tests documentation and v1.0 |

One GitHub repository connects two independent local clones. Your folders and drives can differ. Never work in the same synced checkout on two machines. Each person commits and pushes their own branch; a commit is local until pushed. A pull request asks to combine branch changes into main.

## How the work can happen in parallel

Akshit begins the Part 2 milestones from v0.4. The teammate first builds a standalone tracing prototype against the same base, using syscall 29 while reserving 23 through 28 for threads and futexes. The prototype remains a draft pull request until the validated thread foundation reaches v0.9. Meanwhile the teammate can prepare signal and debugger design notes and test plans.

The complete Part 3 implementation requires the Part 2 interfaces: leader owned memory size, per thread trapframes, thread lifecycle hooks and ptlock. After each Part 2 merge, the teammate merges origin/main into their branch and keeps tracing working. After v0.9, the teammate completes trace inheritance and reset for kernel threads, then integrates signals and debugger milestones in order. This is why both can make progress now without treating the full Part 3 guide as directly runnable on v0.4.

## Set up your local copy

The repository owner first invites your GitHub account under Settings and Collaborators; accept the invitation. Use your own account for authentication and your own author identity. Replace YOUR NAME and YOUR GITHUB EMAIL in the next commands with your actual details. A verified account email or GitHub provided noreply address supports correct attribution.

Open Ubuntu in a directory you choose. The destination xv6-plus must not already contain a different checkout. Prefer a path without spaces on your machine.
    git clone https://github.com/AkshitMishra1107/os_project_xv6_plus.git xv6-plus
    cd xv6-plus
    git config user.name "YOUR NAME"
    git config user.email "YOUR GITHUB EMAIL"
    git fetch origin --tags
    git switch --track origin/feature/signals-debugger
    git branch --show-current
    git rev-parse v0.4

Expected branch: feature/signals-debugger. Expected v0.4 commit: 93fa86df3ddda8691cc2c260b373dd58f726620f. If the branch already exists locally, use git switch feature/signals-debugger instead of creating its tracking branch again. Authentication and author identity are separate; configuring an email does not grant push access.

Check your own versions and record them. The saved owner environment is in docs/memory/evidence/environment-versions.txt; it does not describe the teammate machine.
    git --version
    riscv64-linux-gnu-gcc --version
    qemu-system-riscv64 --version
    gdb-multiarch --version

## Daily edit commit and push routine

Start with a clean working tree on your own branch. Fetch main and merge it before beginning a new chunk. Do not rebase or force push these shared history branches.
    git switch feature/signals-debugger
    git status --short
    git fetch origin
    git merge origin/main

Implement one small logical change. Build and run its focused tests. Save the exact configuration, output and relevant screenshots. Then review all changes and stage the intended source and your evidence directory. The existing ignore rules keep the usual build products out of Git; still inspect the staged list.
    git status --short
    git diff --check
    git diff
    git add kernel user Makefile docs/signals-debugger
    git diff --cached --stat
    git diff --cached --check

Use a descriptive message for the actual completed change. For example:
    git commit -m "trace: add standalone tracing prototype on v0.4"
    git push origin feature/signals-debugger

Several small commits may belong to one milestone PR. Keep incomplete work on your feature branch and push it for backup; describe what is incomplete in the draft PR. Do not tag an unfinished version. A rejected push means fetch and reconcile the branch first; it is not a reason to force push.

## Pull requests review and release tags

On GitHub open Pull requests and New pull request. Set base to main and compare to your own feature branch. Use the milestone as the title. Describe the problem and behavior, shared files changed, exact build options, tests actually run and remaining limits. Use a draft PR while dependencies or checks are incomplete. The other person reviews and reproduces the important checks before a ready PR is merged.

Merge with Create a merge commit. This preserves the individual feature commits and an explicit integration point. Keep both long lived feature branches after each merge so the next milestone continues from the same branch. In the owner role, do not change repository merge policy without first agreeing it with your teammate.

After a PR is merged, update your feature branch and continue. The main version must pass its milestone checks before the integrator creates its official tag. Only Akshit creates release tags, after both people agree that the checks are complete.
    git fetch origin
    git switch feature/signals-debugger
    git merge origin/main
    git push origin feature/signals-debugger

Release procedure example for v0.5. Run this only when the v0.5 PR is merged and the integrated commit has been tested. Check that the tag does not already exist; never replace an existing release tag.
    git fetch origin --tags
    git switch main
    git pull --ff-only
    git status --short
    git tag --list v0.5
    # After the integrated tests pass and no v0.5 tag exists
    git tag -a v0.5 -m "v0.5 thread ready memory validated"
    git push origin refs/tags/v0.5

Use the correct version for each later milestone, then return to feature/signals-debugger. Record the tag commit ID, PR URL and configuration in the release notes. Do not run the example v0.5 tag command while working on a different version.

## Milestone ownership and commit messages

| Release | Deliverable | Suggested milestone commit |
| --- | --- | --- |
| v0.10 | Tracing including thread inheritance | v0.10: integrate syscall tracing with threads |
| v0.11 | Signals and signal library | v0.11: add signals handlers and sigreturn |
| v0.12 | Debugger kernel protocol | v0.12: add debugger kernel support |
| v0.13 | Debugger command line tool | v0.13: add debugger tool and symbolized backtraces |
| v1.0 | Combined demonstration and regression | v1.0: validate and document integrated xv6 Plus |

Before v0.10, use prototype commits such as trace: add standalone tracing prototype on v0.4 and docs: plan signal and debugger integration. These commits are real progress, but they do not establish the final release tag.

## Shared kernel interfaces

| Interface | Agreement |
| --- | --- |
| Syscall registry | memstat 22; thread calls 23 to 26; futex 27 and 28; trace 29; signals 30 to 33; debugger 34 to 39. |
| Process fields | Part 2 supplies leader tid nthreads tidmask and tf_va. The leader owns shared sz and the page table. |
| User copies | At v0.4 pin_user_page returns with interrupts disabled. At v0.5 it must return with ptlock held. Every caller follows the selected contract. |
| Trap return | Part 2 changes per thread trapframe selection and sscratch. Part 3 preserves it while adding signal delivery and instruction cache fences. |
| Trace lifecycle | Copy trace_mask in fork and kthread_create. Reset it in freeproc and reap_thread. Preserve the selected trace state across exec. |
| Scope limits | Swap remains one CPU. Signals run on the main thread. The debugger target is single threaded. No general TLB shootdown is provided. |

The main collision files are kernel/proc.c, kernel/proc.h, kernel/trap.c, kernel/vm.c, kernel/exec.c, kernel/sysproc.c, kernel/sysfile.c, kernel/syscall.c, kernel/syscall.h, kernel/defs.h, user/user.h, user/usys.pl and Makefile. Agree who is editing each shared function for the current milestone. Akshit leads the memory and thread foundation; the teammate adds the Part 3 hooks after those interfaces are available.

## Resolve a merge conflict together

Commit your intended local work first. Run git fetch origin and git merge origin/main. If Git reports conflicts, run git status. Open each named file, remove conflict markers and preserve both features. For syscall tables keep unique numbers; for Makefile keep both object and UPROGS additions. For proc and trap code review control flow, lock ownership and cleanup together. A clean text merge still needs combined runtime checks.
    git status
    # Edit every conflicted file and remove conflict markers
    git add kernel user Makefile docs
    git diff --cached --check
    git diff --cached --stat
    # Build and run the relevant combined checks
    git commit
    # Push your own branch after the merge is complete

If the merge should not proceed, git merge --abort returns to the state before that merge; it is not a way to discard unrelated work. Do not use reset --hard, delete the checkout or overwrite a whole shared file just to silence conflicts.

## Testing modes and evidence

| Mode | Ubuntu build commands | Purpose |
| --- | --- | --- |
| Memory base and trace prototype | make clean then make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1 | Reproduce the established memory base and validate early tracing. |
| Threads and full Part 3 | make clean then make qemu CPUS=3 RAM_MB=128 SWAP_POLICY=1 | Run real parallel threads and normal signal and debugger tests. |
| Swap pressure FIFO | make clean then make qemu CPUS=1 RAM_MB=8 SWAP_POLICY=1 | Run memhog 12 swapfork and hotcold under FIFO. |
| Swap pressure Clock | make clean then make qemu CPUS=1 RAM_MB=8 SWAP_POLICY=2 | Run the same pressure workloads under Clock. |

Exit QEMU with Ctrl A then X before using Ubuntu build or Git commands. Commands shown after an xv6 dollar prompt run inside xv6; do not type that prompt character. Always make clean when configuration changes. RAM_MB limits usable xv6 memory; the current QEMU command still provisions 128 MB.

A terminal transcript can be captured from Ubuntu with script -q docs/signals-debugger/session.txt, followed by the build and xv6 run. After leaving QEMU, type exit to close the capture. Use a new descriptive filename for each milestone so earlier evidence is preserved.
    script -q docs/signals-debugger/v05-session.txt
    # Run the relevant build and test session
    # After Ctrl A then X exits QEMU
    exit

Change v05 in that example to the actual milestone or prototype name. Before committing a transcript, remove accidental credentials if any were typed. Record the real outputs; never paste the expected output in place of an observed test.

For every milestone save docs/signals-debugger/release-notes.md, transcripts and screenshots. Record date, author, branch, commit, CPU count, RAM_MB, SWAP_POLICY, exact test commands, observed results, repeat counts, failures and fixes. Include before and after memstat and explain any persistent difference. A single residual swapped page already occurred in the memory base; it was stable across repeats, and its owner was not traced. Compare repeated equivalent states rather than declaring every nonzero slot a new leak.

Capture screenshots at these points: first successful build and boot; the focused feature success; a useful failure and its repaired rerun; full usertests result; before and after resource readings; the Git commit and tag; the merged PR. Keep configuration and command visible. Preserve raw output as well as images.

## Memory results to preserve

| Observation | Saved result and interpretation |
| --- | --- |
| Lazy reservation | The eager baseline failed sbrk for 200 MB; the lazy version reserved it. lazytest touched 656 pages successfully. Reserved virtual size is not physical usage. |
| Fork comparison | The recorded pre COW run took 38 ticks. COW only runs took 3 to 4 ticks. Final integrated v0.4 took 6 ticks. These are separate configurations and observations. |
| Pressure without swap | At 8 MB usable RAM memhog 4 succeeded and memhog 12 ended with child status minus 1. |
| Pressure with swap | memhog 12 verified 3072 pages with 0 bad and child status 0. Repeated runs and swapfork succeeded. |
| Policy comparison | One measured hotcold phase per policy: FIFO 8006 ins and 8007 outs; Clock 7691 ins and 7684 outs. These predate the final pipe repair and are not timing measurements. |
| Final regression | The complete usertests suite passed after the pipe staging repair. user/usertests.c was not weakened. See the failure and repair evidence in docs/memory. |

## Where the records live

docs/memory/evidence contains the preserved Part 1 screenshots, transcripts, comparisons, summaries and patches. docs/reference holds the original supplied implementation guides. docs/guides holds these two tailored manuals. docs/threads belongs to Akshit; docs/signals-debugger belongs to the teammate; docs/integration holds joint validation and contribution notes. Keep the original sibling report-evidence folder as an additional local record; new shared results belong inside the repository.

## References for the collaboration routine

GitHub repository cloning https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository

GitHub merge methods https://docs.github.com/en/pull-requests/reference/pull-request-merges

GitHub commit email https://docs.github.com/en/account-and-profile/how-tos/email-preferences/setting-your-commit-email-address

## Teammate first runnable tracing prototype

Work on feature/signals-debugger now. This adaptation is explicitly for the saved single threaded v0.4 API. Do not copy the later syscall name table with undefined thread macros into this base. Do not add dummy leader or thread functions. The full source chapter for tracing follows after these adaptation steps.

1. Inspect kernel/proc.h, proc.c, syscall.h, syscall.c, sysproc.c, user/user.h, user/usys.pl and Makefile. Confirm memstat is 22 and 23 through 28 are not repurposed.

2. Add int trace_mask to struct proc. Initialize it to zero in allocproc and clear it in freeproc. Copy it from parent to child in the actual v0.4 fork function. Keep it across exec so a traced command remains traced.

3. Add SYS_trace 29, sys_trace, the handler extern and table entry, the user prototype int trace(int), and entry("trace") in usys.pl. The current argint helper returns void; use the supplied sys_trace style rather than testing an int return.

4. Create user/trace.c from the complete source listing and add $U/_trace to UPROGS with the surrounding Makefile continuation syntax.

5. Create syscall_names entries only for currently defined calls 1 through 22 and trace 29. Leave the reserved gaps empty. Preserve syscall dispatch range and nonnull handler checks. Guard the name lookup as shown below before printing.

6. Build at CPUS=1 RAM_MB=128 SWAP_POLICY=1. Run trace 32 cat README, trace 128 echo hi, trace 2 sh then ls and exit, and a mask with no matching calls. Verify normal commands still behave correctly.

7. Run lazytest cowtest forkbench and usertests. Save the prototype transcript and before and after memstat. Commit and push the prototype, then open a draft PR to main; leave it draft until v0.9 and all thread hooks are integrated.

Inside the existing successful syscall dispatch branch, after assigning its return value to a0, the prototype can use this guarded print. Add the current syscall_names table separately; keep the existing unknown syscall branch unchanged.
    if(num < 31 && (p->trace_mask & (1U << num))){
      const char *name = "unknown";
      if(num < NELEM(syscall_names) && syscall_names[num])
        name = syscall_names[num];
      printf("%d: syscall %s -> %d\n", p->pid, name,
             (int)p->trapframe->a0);
    }

The int mask covers syscall numbers 1 through 30. Signals and debugger use numbers above 30 too; they will not all be traced with this design. Do not silently advertise full tracing of 39 calls. A wider mask is a separate API change requiring coordinated prototypes, argument parsing and tests.

## Upgrade tracing as the thread base arrives

Commit your work, fetch origin, and merge origin/main after each validated Part 2 milestone. Add the real kthread_create inheritance and reap_thread reset as those functions arrive. Add syscall name entries 23 through 28 when their definitions exist. Preserve the per thread trapframe path and the current syscall dispatcher. At v0.9 run both process child and kernel thread trace inheritance checks, then make the trace PR ready. Only the reviewed integrated release receives v0.10.

## Prepare signals and debugger without blocking threads

While Akshit builds the foundation, write the pending signal model, restorer flow, debugger stop protocol and test cases in docs/signals-debugger. Keep unintegrated sketches as documentation rather than half wired kernel modules. Once v0.9 is merged, follow the complete implementation chapters in order. Changes to proc lifecycle, exec and trap return need joint review.

The v0.12 copyout_force listing assumes pin_user_page returns with ptlock held. The saved v0.4 function instead leaves interrupts disabled. Using that listing unchanged on v0.4 would release an unheld lock. Confirm the new contract in the actual main branch before wiring debugger memory writes. Preserve the v0.4 pipe repair and Part 2 trampoline changes.

## Part 3 acceptance gates

| Version | Checks before its PR is ready |
| --- | --- |
| v0.10 | Read and exec trace masks plus child inheritance and thread inheritance after v0.9; reserved numbering intact; earlier regressions. |
| v0.11 | sigtest five success checks at least ten runs; ignored and default actions; sleep interruption; full trapframe restoration and restored a0; usertests. |
| v0.12 | dbgmini at least ten runs with exec stop reason 1 and exit reason 3; authorized stopped child access; orphan and quit cleanup; usertests. |
| v0.13 | dbg dbgtarget break middle and inner; bt regs info continue; final result 12; invalid and duplicate function handling; no frozen target after quit. |
| v1.0 | finaldemo 1536 pages and signal 10; every earlier feature test; full usertests; one CPU FIFO and Clock pressure checks; both machines reproduce the documented modes. |

Signals are delivered to the main thread at return to user mode. The debugger supports one single threaded child and function entry breakpoints, with both compressed two byte and ordinary four byte instructions. Do not claim POSIX signals or a general GDB replacement. For finaldemo fork the signaling child before creating threads, as the source listing does.

## Implementation chapters for signals tracing and debugger

Complete concepts, source listings, tests and troubleshooting adapted from the supplied Part 3 guide. The shared workflow and compatibility notes above take precedence over the original team schedule and release commands. Implement and validate each milestone on your assigned feature branch.

### 1  What you will build

Part 3 gives programs three new abilities and gives you the tools to look inside a running program. Each release works on its own.

| Release | Feature | What it gives you |
| --- | --- | --- |
| v0.10 | trace system call | print every system call a program makes, like Linux's strace |
| v0.11 | Signals | one process can notify another, and the receiver runs a handler function (how Ctrl-C works on Linux) |
| v0.12 | Debugger support in the kernel | a parent can stop, inspect and resume a child program |
| v0.13 | The dbg tool | breakpoints on functions, registers, and a call stack (backtrace) with function names |
| v1.0 | Demos, integration, final polish | all three parts working together, ready for the final demo |

Suggested implementation sequence

| Stage | Work |
| --- | --- |
| 1 | Read this guide, do trace (v0.10), start signals |
| 2 | Signals (v0.11) |
| 3 | Debugger kernel support (v0.12) |
| 4 | The dbg tool: symbols, breakpoints (v0.13) |
| 5 | Backtrace, resume logic, demos, bug fixing |
| 6 | Integration test of all three parts, final report, demo video |

Akshit implements all Part 2 milestones. The teammate implements Part 3. Both review kernel interfaces, run integration checks and contribute their report sections. Work by acceptance gate rather than the original three person six week schedule.

### 2  Before you start

Full Part 3 integration requires Part 2 finished and tagged v0.9. Continue on feature/signals-debugger after merging that validated main base. Until then follow the standalone tracing adaptation above.

Use the default make qemu (3 CPUs, 128 MB) for everything in this part. Swap from Part 1 still needs CPUS=1 RAM_MB=8.

As before, function names and signatures differ a little between xv6 versions. Open the real function before you edit it.

Part 3 touches the trap path in kernel/trap.c, syscall() in kernel/syscall.c, exec and exit. Commit often, because a mistake there can stop the kernel from booting.

### 3  Concepts you need

#### 3 1 Signals

A signal is a small numbered message sent to a process by the kernel or by another process. Pressing Ctrl-C in a Linux terminal sends signal 2 (SIGINT) to the running program. A program can register a handler: a function the kernel runs when the signal arrives. If it registered nothing, the default action happens (usually: the program ends).

The kernel never interrupts a program in the middle of a kernel operation. It records the signal as pending in the receiver, and delivers it the next time the receiver is about to return to user mode. Delivery works like this:

Save all the registers of the interrupted program.

Change the program counter so the program "jumps" into the handler.

When the handler returns, a small restorer function calls the sigreturn system call.

sigreturn restores the saved registers, so the program continues exactly where it was interrupted.

#### 3 2 System call tracing

Every system call passes through one function in the kernel (syscall()). Tracing is just a check there: if this process asked to be traced for this call, print its name and result.

#### 3 3 How a debugger works

A debugger is a program that controls another program (the target). It needs these abilities:

stop the target at a chosen place

read and write the target's memory and registers

resume it

A breakpoint is a trick: the debugger overwrites the instruction at the chosen address with a special instruction, ebreak, which means "stop here". When the target executes it, the CPU raises a breakpoint trap (cause number 3) and enters the kernel. The kernel freezes the target and tells the debugger. To continue, the debugger puts the original instruction back.

RISC-V instructions are either 4 bytes or, when "compressed", 2 bytes. The breakpoint instruction has the same two sizes: ebreak (4 bytes) and c.ebreak (2 bytes). We use the size that matches the instruction we overwrite.

#### 3 4 Call stacks

When a function is called, the compiler saves the return address and the caller's frame pointer (register s0, also called fp) on the stack. In xv6 all code is compiled with -fno-omit-frame-pointer, so these saved values form a chain:
    fp - 8   saved return address (where to go back in the caller)
    fp - 16  saved frame pointer  (the caller's own frame)

Following the chain gives the list of active function calls, the backtrace. At the first instruction of a function the new frame is not yet built, so the return address is still in register ra and fp still belongs to the caller.

#### 3 5 Function names from the program file

The addresses in a backtrace are just numbers. User programs in xv6 keep their ELF symbol table, which lists every function with its address and size, so the debugger can print names.

### 4  Design

Signals. All signal state lives in the process's main thread (p->leader): pending bits, one handler per signal, and room to save the interrupted registers. Delivery happens in usertrap(), right before the return to user mode, and only for the main thread. Threads other than the main thread never run handlers (limitation).

Debugger. The debugger is an ordinary user program, dbg. It forks, the child calls dbg_traceme() and then execs the program to debug. The kernel stops that child right after exec. The debugger then uses six new system calls to control it:

| System call | Purpose |
| --- | --- |
| dbg_traceme() | the calling process agrees to be controlled by its parent |
| dbg_wait(pid) | sleep until the child stops or exits, and say why |
| dbg_cont(pid) | resume a stopped child |
| dbg_getregs(pid, &regs) | read the stopped child's registers |
| dbg_peek(pid, addr, buf, n) | read the child's memory |
| dbg_poke(pid, addr, buf, n) | write the child's memory, even read-only code pages |

Files you will touch

| File | Change |
| --- | --- |
| kernel/sigdefs.h (new) | signal numbers, shared with user programs |
| kernel/signal.c (new) | signal state, delivery, sigaction, sigsend, sigreturn, pause |
| kernel/dbg.h, kernel/dbg.c (new) | debugger system calls and stop logic |
| kernel/proc.h, kernel/proc.c | new fields; fork, exit, freeproc changes |
| kernel/trap.c | deliver signals; handle the breakpoint trap |
| kernel/syscall.c, kernel/syscall.h, kernel/sysproc.c | trace, new system calls, sleep interruption |
| kernel/exec.c, kernel/sysfile.c | reset signal handlers; stop a traced child after exec |
| kernel/vm.c | copyout_force (write into code pages) |
| user/*.c (new) | trace, sig library, dbg, demo programs |

Use the standalone adaptation above while on the v0.4 base. The complete chapter below assumes the v0.9 thread and futex interfaces are present. Final v0.10 includes both process and kernel thread inheritance.

### 5  Milestone 10  the trace system call

Idea. A process can ask for a list of system calls to be printed. The list is a bit mask: bit n set means "trace system call number n". The mask is a field in struct proc. It is copied to children, so a traced shell command stays traced.

#### Step 1  new field

Add to struct proc:
      int trace_mask;              // bit n set: print system call number n

In freeproc() (and in reap_thread() from Part 2) add p->trace_mask = 0; next to the other resets.

In fork() copy it to the child, next to safestrcpy(np->name, ...):
      np->trace_mask = p->trace_mask;

Do the same in kthread_create() so new threads are traced too: np->trace_mask = p->trace_mask;.

#### Step 2  the system call
    uint64
    sys_trace(void)
    {
      int mask;

      argint(0, &mask);
      myproc()->trace_mask = mask;
      return 0;
    }

Do the usual plumbing, continuing your numbering:

| File | Edit |
| --- | --- |
| kernel/syscall.h | #define SYS_trace 29 |
| kernel/syscall.c | extern uint64 sys_trace(void); and [SYS_trace] sys_trace, in the table |
| user/usys.pl | entry("trace"); |
| user/user.h | int trace(int); |

#### Step 3  print the calls

Add a table of names, indexed by system call number. Put it before syscall(). It uses the same designated-initializer style as the syscalls table:
    static char *syscall_names[] = {
    [SYS_fork]           "fork",
    [SYS_exit]           "exit",
    [SYS_wait]           "wait",
    [SYS_pipe]           "pipe",
    [SYS_read]           "read",
    [SYS_kill]           "kill",
    [SYS_exec]           "exec",
    [SYS_fstat]          "fstat",
    [SYS_chdir]          "chdir",
    [SYS_dup]            "dup",
    [SYS_getpid]         "getpid",
    [SYS_sbrk]           "sbrk",
    [SYS_sleep]          "sleep",
    [SYS_uptime]         "uptime",
    [SYS_open]           "open",
    [SYS_write]          "write",
    [SYS_mknod]          "mknod",
    [SYS_unlink]         "unlink",
    [SYS_link]           "link",
    [SYS_mkdir]          "mkdir",
    [SYS_close]          "close",
    [SYS_memstat]        "memstat",
    [SYS_kthread_create] "kthread_create",
    [SYS_kthread_exit]   "kthread_exit",
    [SYS_kthread_join]   "kthread_join",
    [SYS_kgettid]        "kgettid",
    [SYS_futex_wait]     "futex_wait",
    [SYS_futex_wake]     "futex_wake",
    [SYS_trace]          "trace",
    };

Every time you add a system call later in this part, add its line here too. Then change syscall():
    void
    syscall(void)
    {
      int num;
      struct proc *p = myproc();

      num = p->trapframe->a7;
      if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
        p->trapframe->a0 = syscalls[num]();
        // print the call if this process asked for it (the mask covers calls 1..30)
        if(num < 31 && (p->trace_mask & (1 << num)))
          printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num],
                 (int)p->trapframe->a0);
      } else {
        printf("%d %s: unknown sys call %d\n", p->pid, p->name, num);
        p->trapframe->a0 = -1;
      }
    }

Keep the exact lookup lines your version already has. The only additions are the if(num < 31 ...) block and the table. The limit of 31 exists because the mask is an int.

#### Step 4  the command
    #include "kernel/types.h"
    #include "user/user.h"

    int
    main(int argc, char *argv[])
    {
      if(argc < 3){
        printf("usage: trace mask command [args]\n");
        exit(1);
      }
      if(trace(atoi(argv[1])) < 0){
        printf("trace: system call failed\n");
        exit(1);
      }
      exec(argv[2], &argv[2]);
      printf("trace: exec %s failed\n", argv[2]);
      exit(1);
    }

Add $U/_trace\ to UPROGS.

#### Step 5  build  run  verify
    make clean
    make qemu

The mask for system call n is 1 << n. Useful values with the standard numbering: read = 5 gives 32, write = 16 gives 65536, fork = 1 gives 2, exec = 7 gives 128, open = 15 gives 32768.
    $ trace 32 cat README
    4: syscall read -> 1023
    (file text)
    4: syscall read -> 961
    (file text)
    4: syscall read -> 0
    $ trace 128 echo hi
    5: syscall exec -> 2
    hi
    $ trace 2147483647 echo hi
    (every system call that echo makes, up to number 30)

The process ids and byte counts will differ. Check these three things:

the read lines show the sizes that cat actually read, ending with 0

a command that makes no matching call prints nothing extra

fork children inherit the trace: trace 2 sh followed by running ls inside prints syscall fork lines

Commit the reviewed source and evidence on feature/signals-debugger, push it and open the milestone PR. Suggested message: v0.10: trace. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

Common mistakes

| Symptom | Cause |
| --- | --- |
| nothing is printed | the mask is cleared by exec, or trace_mask is not copied in fork |
| garbage names or a crash in printf | a syscall_names entry is missing for a number that is being traced (it is 0, a null pointer) |
| compile error for SYS_trace | syscall.h edit missing |
| the mask works only for small numbers | 1 << num overflows an int above 30; that is why the limit is 31 |

### 6  Milestone 11  signals

Idea. sigsend(pid, sig) sets a bit in the receiver's sig_pending mask. Just before the receiver returns to user mode, the kernel looks at the mask. If a handler is registered, the kernel saves the whole trapframe, points the program counter at the handler and puts the signal number in a0. The handler's return address is a user-side "restorer" that calls sigreturn, which copies the saved trapframe back. If no handler is registered, the default action terminates the process.

Continue on feature/signals-debugger. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### Step 1  signal numbers

User programs include this file too.
    #define NSIG     16
    #define SIGINT    2
    #define SIGKILL   9
    #define SIGUSR1  10
    #define SIGUSR2  12
    #define SIGALRM  14
    #define SIGTERM  15
    #define SIG_DFL   0      // default action: terminate the process
    #define SIG_IGN   1      // ignore the signal

#### Step 2  per process signal state

At the very top of proc.h add #include "sigdefs.h". Then add to struct proc:
      uint32 sig_pending;           // main thread only: bit n set = signal n is waiting
      uint64 sig_handler[NSIG];     // per signal: SIG_DFL, SIG_IGN or a user function address
      uint64 sig_restorer[NSIG];    // user function that calls sigreturn
      struct trapframe sig_saved;   // the interrupted registers while a handler runs
      int in_handler;               // 1 while a handler runs

All signal state lives in the main thread (p->leader), so one process has one set of handlers.

In freeproc() add the resets (a loop is fine):
      p->sig_pending = 0;
      p->in_handler = 0;
      for(int s = 0; s < NSIG; s++){
        p->sig_handler[s] = 0;
        p->sig_restorer[s] = 0;
      }

fork(): the child keeps the parent's handlers but starts with nothing pending. After the trace_mask line add:
      for(i = 0; i < NSIG; i++){
        np->sig_handler[i] = l->sig_handler[i];
        np->sig_restorer[i] = l->sig_restorer[i];
      }

exec(): a new program must not keep function addresses from the old one. After the line that commits the new image (p->sz = sz;) add:
      for(int s = 0; s < NSIG; s++){          // caught signals go back to default
        if(p->sig_handler[s] != SIG_IGN)
          p->sig_handler[s] = SIG_DFL;
        p->sig_restorer[s] = 0;
      }
      p->sig_pending = 0;
      p->in_handler = 0;

#### Step 3  the signal code
    #include "types.h"
    #include "param.h"
    #include "memlayout.h"
    #include "riscv.h"
    #include "spinlock.h"
    #include "proc.h"
    #include "defs.h"

    extern struct proc proc[NPROC];

    struct spinlock siglock;         // protects the signal state of all processes

    void
    signal_init(void)
    {
      initlock(&siglock, "siglock");
    }

    // Is a signal waiting for this thread to handle? Only the main thread handles signals.
    int
    signal_pending(struct proc *p)
    {
      return p == p->leader && p->sig_pending != 0 && !p->in_handler;
    }

    // Called from usertrap() just before returning to user mode.
    void
    deliver_signals(struct proc *p)
    {
      if(p->sig_pending == 0 || p != p->leader || p->in_handler)
        return;

      acquire(&siglock);
      while(p->sig_pending != 0){
        int sig = 1;
        while((p->sig_pending & (1 << sig)) == 0)
          sig++;
        p->sig_pending &= ~(1 << sig);
        uint64 handler = p->sig_handler[sig];
        uint64 restorer = p->sig_restorer[sig];

        if(handler == SIG_IGN)
          continue;                         // ignored: look at the next one
        release(&siglock);

        if(handler == SIG_DFL)
          exit(-1);                         // default action: terminate the process

        // Run the handler: save the interrupted registers, then make the
        // program "call" handler(sig) that returns into the restorer.
        p->sig_saved = *p->trapframe;
        p->in_handler = 1;
        p->trapframe->epc = handler;
        p->trapframe->a0 = sig;
        p->trapframe->ra = restorer;
        return;
      }
      release(&siglock);
    }

    // Send signal sig to the process with this pid.
    int
    sigsend(int pid, int sig)
    {
      if(sig <= 0 || sig >= NSIG)
        return -1;
      if(sig == SIGKILL)
        return kill(pid);                   // SIGKILL cannot be caught

      acquire(&siglock);
      for(struct proc *p = proc; p < &proc[NPROC]; p++){
        if(p->pid == pid && p == p->leader &&
           p->state != UNUSED && p->state != ZOMBIE){
          p->sig_pending |= (1 << sig);
          wakeup(p);                        // wake a sleeping pause() so it notices
          release(&siglock);
          return 0;
        }
      }
      release(&siglock);
      return -1;
    }

    uint64
    sys_sigaction(void)
    {
      int sig;
      uint64 handler, restorer;
      struct proc *l = myproc()->leader;

      argint(0, &sig);
      argaddr(1, &handler);
      argaddr(2, &restorer);
      if(sig <= 0 || sig >= NSIG || sig == SIGKILL)
        return -1;
      acquire(&siglock);
      l->sig_handler[sig] = handler;
      l->sig_restorer[sig] = restorer;
      release(&siglock);
      return 0;
    }

    uint64
    sys_sigsend(void)
    {
      int pid, sig;

      argint(0, &pid);
      argint(1, &sig);
      return sigsend(pid, sig);
    }

    // Called by the restorer when a handler returns: put the registers back.
    uint64
    sys_sigreturn(void)
    {
      struct proc *p = myproc();

      if(!p->in_handler)
        return -1;
      *p->trapframe = p->sig_saved;
      p->in_handler = 0;
      return p->trapframe->a0;             // syscall() stores this in a0: it must be the OLD a0
    }

    // Sleep until a signal arrives. Always returns -1, like Unix pause().
    uint64
    sys_pause(void)
    {
      struct proc *p = myproc();

      acquire(&siglock);
      while(!signal_pending(p) && !killed(p))
        sleep(p->leader, &siglock);         // woken by sigsend
      release(&siglock);
      return -1;
    }

Read these three details twice; they are where students go wrong.

sys_sigreturn returns the restored a0. After every system call, syscall() writes the return value into a0. If sigreturn returned 0, it would overwrite the interrupted program's a0. Returning the restored a0 makes that write harmless.

sigsend wakes p, not just sets the bit. Without the wakeup a pause() that checked the mask just before the signal arrived would sleep forever. The check and the sleep happen under siglock, and sigsend takes the same lock before waking.

Delivery happens after the return value is set. The handler runs when the program returns from the system call that was interrupted, so a handler never sees half-finished kernel work.

#### Step 4  deliver on the way back to user mode

In usertrap(), right before usertrapret(); (after the timer yield()):
      deliver_signals(p);

      usertrapret();

#### Step 5  let signals interrupt sleep

In sys_sleep, change the check inside the loop so a pending signal ends the sleep:
        if(killed(myproc()) || signal_pending(myproc())){
          release(&tickslock);
          return -1;
        }

#### Step 6  declarations  boot and system call plumbing

kernel/defs.h: add
    // signal.c
    void            signal_init(void);
    void            deliver_signals(struct proc*);
    int             signal_pending(struct proc*);
    int             sigsend(int, int);

kernel/main.c: call signal_init(); after futex_init();

Makefile: add $K/signal.o \ to OBJS

| File | Edit |
| --- | --- |
| kernel/syscall.h | #define SYS_sigaction 30, SYS_sigsend 31, SYS_sigreturn 32, SYS_pause 33 |
| kernel/syscall.c | extern uint64 sys_sigaction(void); and so on, the four table entries, and the four names ("sigaction", "sigsend", "sigreturn", "pause") in syscall_names |
| user/usys.pl | entry("sigaction");, entry("sigsend");, entry("sigreturn");, entry("pause"); |
| user/user.h | the four prototypes below |
    int sigaction(int, uint64, uint64);
    int sigsend(int, int);
    int sigreturn(void);
    int pause(void);

#### Step 7  the user library

user/sig.h:
    #ifndef USER_SIG_H
    #define USER_SIG_H

    #include "kernel/sigdefs.h"

    // handler is a function address, or SIG_DFL / SIG_IGN
    int signal(int sig, uint64 handler);

    #endif

user/sig.c:
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/sig.h"

    // When a handler returns, it "returns" here: the kernel put this address
    // in the return-address register before running the handler.
    static void
    sig_restorer(void)
    {
      sigreturn();
    }

    int
    signal(int sig, uint64 handler)
    {
      return sigaction(sig, handler, (uint64)sig_restorer);
    }

Append $U/sig.o to the ULIB line in the Makefile, next to thread.o.

#### Step 8  test
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/sig.h"

    static volatile int got;                // set by the handler

    static void
    on_usr1(int sig)
    {
      got = sig;
    }

    static void
    on_usr1_child(int sig)
    {
      printf("child: got signal %d\n", sig);
      exit(0);
    }

    int
    main(void)
    {
      int pid, status;

      // 1. a signal sent to ourselves runs the handler, then the program goes on
      signal(SIGUSR1, (uint64)on_usr1);
      int before = 12345;                   // a local variable must survive the handler
      sigsend(getpid(), SIGUSR1);
      if(got != SIGUSR1 || before != 12345){
        printf("sigtest: FAIL 1\n");
        exit(1);
      }
      printf("sigtest: 1 ok, signal to self\n");

      // 2. a signal wakes pause() in another process
      pid = fork();
      if(pid == 0){
        signal(SIGUSR1, (uint64)on_usr1_child);
        pause();
        exit(1);                            // not reached: the handler exits
      }
      sleep(5);
      sigsend(pid, SIGUSR1);
      wait(&status);
      if(status != 0){
        printf("sigtest: FAIL 2\n");
        exit(1);
      }
      printf("sigtest: 2 ok, signal to another process\n");

      // 3. default action: a signal without a handler ends the process
      pid = fork();
      if(pid == 0){
        for(;;)
          ;                                 // spin until killed
      }
      sleep(5);
      sigsend(pid, SIGTERM);
      wait(&status);
      if(status == 0){
        printf("sigtest: FAIL 3\n");
        exit(1);
      }
      printf("sigtest: 3 ok, default action ended the child\n");

      // 4. an ignored signal does nothing
      signal(SIGUSR2, SIG_IGN);
      sigsend(getpid(), SIGUSR2);
      printf("sigtest: 4 ok, ignored signal\n");

      // 5. a signal interrupts sleep(), and the return value survives the handler
      pid = fork();
      if(pid == 0){
        signal(SIGUSR1, (uint64)on_usr1);
        int r = sleep(1000);                // would take about 100 seconds
        exit(r == -1 && got == SIGUSR1 ? 0 : 1);
      }
      sleep(5);
      sigsend(pid, SIGUSR1);
      wait(&status);
      if(status != 0){
        printf("sigtest: FAIL 5\n");
        exit(1);
      }
      printf("sigtest: 5 ok, signal interrupted sleep\n");

      printf("sigtest: ALL OK\n");
      exit(0);
    }

Add $U/_sigtest\ to UPROGS.

#### Step 9  build  run  verify
    make clean
    make qemu
    $ sigtest

Expected: the five ok lines (with child: got signal 10 inside test 2) and sigtest: ALL OK. Run it ten times. Also run usertests to check nothing else broke.

Commit the reviewed source and evidence on feature/signals-debugger, push it and open the milestone PR. Suggested message: v0.11: signals. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

Common mistakes

| Symptom | Cause |
| --- | --- |
| test 1 fails, handler never runs | deliver_signals not called in usertrap, or the delivery check returns early for the main thread |
| program crashes after the handler returns | sigreturn not restoring the whole trapframe, or the restorer address was not passed (sigaction third argument) |
| test 1 passes but a later value is wrong | sys_sigreturn returned 0 instead of the restored a0 |
| pause() hangs even though the signal was sent | sigsend does not call wakeup(p) while holding siglock, or pause sleeps on a different channel than p->leader |
| test 5 hangs for 100 seconds | sys_sleep does not check signal_pending |
| panic: acquire | deliver_signals keeps siglock while calling exit or the handler setup |
| a forked child jumps to a garbage address on a signal | exec does not reset the handlers |

### 7  Milestone 12  debugger support in the kernel

Idea. The kernel gives a parent the power to freeze, inspect and resume one of its children. The child opts in with dbg_traceme(). After that:

the child stops right after exec, before running its first instruction, so the debugger can plant breakpoints

a breakpoint instruction (ebreak) in a traced child stops it instead of killing it

while the child is stopped, the parent can read its registers, read and write its memory, and resume it

Continue on feature/signals-debugger. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### Step 1  shared definitions
    // Reasons returned by dbg_wait.
    #define DBG_EXEC   1     // stopped right after exec, before the first instruction
    #define DBG_BREAK  2     // stopped at a breakpoint
    #define DBG_EXITED 3     // the program has exited

    struct dbgregs {
      uint64 pc, ra, sp, fp;
      uint64 a0, a1, a2, a3, a4, a5, a6, a7;
    };

In kernel/proc.h add two fields to struct proc:
      int traced;                  // 1 if the parent is allowed to control this process
      int dbg_state;               // 0 running; otherwise the reason it is stopped

In freeproc() add p->traced = 0; p->dbg_state = 0;. (allocproc needs nothing, because freeproc leaves the fields zero. A child created by fork starts untraced.)

#### Step 2  writing into code pages

Program code is mapped read-only, so copyout refuses to write it. A debugger must. Add this function next to copyout:
    // Write into another process's memory even if the page is read-only (code).
    // Refuses pages shared with another process, because the write would be
    // visible there too.
    int
    copyout_force(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
    {
      uint64 n, va0, pa0;

      while(len > 0){
        va0 = PGROUNDDOWN(dstva);
        pa0 = pin_user_page(pagetable, va0, 0);   // resident, user page; returns with ptlock held
        if(pa0 == 0)
          return -1;
        if(kref_get((void*)pa0) != 1){
          release(&ptlock);
          return -1;
        }
        n = PGSIZE - (dstva - va0);
        if(n > len)
          n = len;
        memmove((void *)(pa0 + (dstva - va0)), src, n);
        release(&ptlock);

        len -= n;
        src += n;
        dstva = va0 + PGSIZE;
      }
      asm volatile("fence.i");                   // the CPU must see the new instructions
      return 0;
    }

In kernel/defs.h add int copyout_force(pagetable_t, uint64, char*, uint64); in the vm.c section.

#### Step 3  the debugger system calls

All stop and resume state is protected by wait_lock, the same lock exit and wait use. That way a debugger sleeping in dbg_wait is woken both by a stop and by the child's exit.
    #include "types.h"
    #include "param.h"
    #include "memlayout.h"
    #include "riscv.h"
    #include "spinlock.h"
    #include "proc.h"
    #include "defs.h"
    #include "dbg.h"

    extern struct proc proc[NPROC];
    extern struct spinlock wait_lock;

    #define DBG_MAXIO 64

    // The target must be a traced child of the caller. Call with wait_lock held.
    static struct proc *
    find_target(int pid)
    {
      struct proc *l = myproc()->leader;

      for(struct proc *t = proc; t < &proc[NPROC]; t++){
        if(t->pid == pid && t->parent == l && t->traced && t->state != UNUSED)
          return t;
      }
      return 0;
    }

    // A target that is currently stopped, or 0.
    static struct proc *
    stopped_target(int pid)
    {
      acquire(&wait_lock);
      struct proc *t = find_target(pid);
      if(t && t->dbg_state == 0)
        t = 0;
      release(&wait_lock);
      return t;
    }

    // Freeze the calling (traced) process and tell its debugger.
    // Returns when the debugger calls dbg_cont.
    void
    dbg_stop(struct proc *p, int reason)
    {
      acquire(&wait_lock);
      p->dbg_state = reason;
      wakeup(p->parent);                    // wake the debugger sleeping in dbg_wait
      while(p->dbg_state != 0 && !killed(p))
        sleep(&p->dbg_state, &wait_lock);
      p->dbg_state = 0;
      release(&wait_lock);
    }

    uint64
    sys_dbg_traceme(void)
    {
      myproc()->traced = 1;
      return 0;
    }

    // Sleep until the child stops or exits. Returns the reason, or -1.
    uint64
    sys_dbg_wait(void)
    {
      int pid;
      struct proc *l = myproc()->leader;

      argint(0, &pid);
      acquire(&wait_lock);
      for(;;){
        struct proc *t = find_target(pid);
        if(t == 0 || killed(myproc())){
          release(&wait_lock);
          return -1;
        }
        if(t->dbg_state != 0){
          int why = t->dbg_state;
          release(&wait_lock);
          return why;
        }
        acquire(&t->lock);
        int dead = (t->state == ZOMBIE);
        release(&t->lock);
        if(dead){
          release(&wait_lock);
          return DBG_EXITED;
        }
        sleep(l, &wait_lock);               // woken by dbg_stop or by the child's exit
      }
    }

    uint64
    sys_dbg_cont(void)
    {
      int pid;

      argint(0, &pid);
      acquire(&wait_lock);
      struct proc *t = find_target(pid);
      if(t == 0 || t->dbg_state == 0){
        release(&wait_lock);
        return -1;
      }
      t->dbg_state = 0;
      wakeup(&t->dbg_state);
      release(&wait_lock);
      return 0;
    }

    uint64
    sys_dbg_getregs(void)
    {
      int pid;
      uint64 uregs;
      struct dbgregs r;

      argint(0, &pid);
      argaddr(1, &uregs);
      struct proc *t = stopped_target(pid);
      if(t == 0)
        return -1;
      struct trapframe *tf = t->trapframe;
      r.pc = tf->epc;
      r.ra = tf->ra;
      r.sp = tf->sp;
      r.fp = tf->s0;
      r.a0 = tf->a0;
      r.a1 = tf->a1;
      r.a2 = tf->a2;
      r.a3 = tf->a3;
      r.a4 = tf->a4;
      r.a5 = tf->a5;
      r.a6 = tf->a6;
      r.a7 = tf->a7;
      if(copyout(myproc()->pagetable, uregs, (char*)&r, sizeof(r)) < 0)
        return -1;
      return 0;
    }

    // dbg_peek(pid, addr, buf, n): copy n bytes from the child's memory.
    uint64
    sys_dbg_peek(void)
    {
      int pid, n;
      uint64 addr, ubuf;
      char kbuf[DBG_MAXIO];

      argint(0, &pid);
      argaddr(1, &addr);
      argaddr(2, &ubuf);
      argint(3, &n);
      if(n <= 0 || n > DBG_MAXIO)
        return -1;
      struct proc *t = stopped_target(pid);
      if(t == 0)
        return -1;
      if(copyin(t->pagetable, kbuf, addr, n) < 0)
        return -1;
      return copyout(myproc()->pagetable, ubuf, kbuf, n) < 0 ? -1 : 0;
    }

    // dbg_poke(pid, addr, buf, n): copy n bytes into the child's memory, even code.
    uint64
    sys_dbg_poke(void)
    {
      int pid, n;
      uint64 addr, ubuf;
      char kbuf[DBG_MAXIO];

      argint(0, &pid);
      argaddr(1, &addr);
      argaddr(2, &ubuf);
      argint(3, &n);
      if(n <= 0 || n > DBG_MAXIO)
        return -1;
      struct proc *t = stopped_target(pid);
      if(t == 0)
        return -1;
      if(copyin(myproc()->pagetable, kbuf, ubuf, n) < 0)
        return -1;
      return copyout_force(t->pagetable, addr, kbuf, n) < 0 ? -1 : 0;
    }

A stopped child cannot change: it is asleep inside dbg_stop until its parent calls dbg_cont. That is why it is safe to read its trapframe and page table after stopped_target has released the lock.

#### Step 4  stop at exec  at breakpoints  and clean up

kernel/sysfile.c, in sys_exec. Stop the traced child right after a successful exec, before it runs:
      int ret = exec(path, argv);

      for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
        kfree(argv[i]);

      if(ret >= 0 && myproc()->traced)
        dbg_stop(myproc(), DBG_EXEC);       // let the debugger plant breakpoints first
      return ret;

Add #include "dbg.h" at the top of sysfile.c.

kernel/trap.c. Add #include "dbg.h". In usertrap(), add a branch for the breakpoint trap (cause 3) before the page-fault branch:
      } else if(r_scause() == 3 && p->traced){
        // ebreak in a traced process: freeze here; epc still points at the ebreak
        dbg_stop(p, DBG_BREAK);
      } else if(r_scause() == 12 || r_scause() == 13 || r_scause() == 15){

An ebreak in a process that is not traced still falls through to "unexpected scause" and kills it, which is the correct behavior.

In usertrapret(), just before the final call into the trampoline, make the CPU see code that the debugger changed:
      if(p->traced)
        asm volatile("fence.i");

kernel/proc.c, in reparent(). If a debugger exits while its child is frozen, the child would stay frozen forever. Kill it:
        if(pp->parent == p){
          pp->parent = initproc;
          if(pp->traced){                   // do not leave a frozen child behind
            pp->traced = 0;
            pp->dbg_state = 0;
            wakeup(&pp->dbg_state);
            kill(pp->pid);
          }
          wakeup(initproc);
        }

#### Step 5  plumbing for six more system calls

| File | Edit |
| --- | --- |
| kernel/syscall.h | SYS_dbg_traceme 34, SYS_dbg_wait 35, SYS_dbg_cont 36, SYS_dbg_getregs 37, SYS_dbg_peek 38, SYS_dbg_poke 39 |
| kernel/syscall.c | six extern lines, six table entries, six names in syscall_names |
| kernel/defs.h | void dbg_stop(struct proc*, int); |
| kernel/main.c | nothing to initialize: wait_lock already exists |
| Makefile | add $K/dbg.o \ to OBJS |
| user/usys.pl | six entry("dbg_..."); lines |
| user/user.h | struct dbgregs; near the top, and the six prototypes below |
    int dbg_traceme(void);
    int dbg_wait(int);
    int dbg_cont(int);
    int dbg_getregs(int, struct dbgregs*);
    int dbg_peek(int, uint64, void*, int);
    int dbg_poke(int, uint64, void*, int);

syscall_names has now grown to cover numbers up to 39. The trace mask still only covers numbers up to 30, as explained in Milestone 10.

#### Step 6  a quick kernel side test

Before writing the full tool, check the six system calls with a tiny program:
    #include "kernel/types.h"
    #include "user/user.h"
    #include "kernel/dbg.h"

    int
    main(void)
    {
      int pid = fork();
      if(pid == 0){
        dbg_traceme();
        char *args[] = { "echo", "hello from the target", 0 };
        exec("echo", args);
        exit(1);
      }

      int why = dbg_wait(pid);
      struct dbgregs r;
      uchar insn[4];
      dbg_getregs(pid, &r);
      dbg_peek(pid, r.pc, insn, 4);
      printf("dbgmini: stopped, reason %d, pc %p, sp %p\n", why, r.pc, r.sp);
      printf("dbgmini: bytes at pc: %x %x %x %x\n", insn[0], insn[1], insn[2], insn[3]);

      dbg_cont(pid);
      why = dbg_wait(pid);
      printf("dbgmini: final reason %d (3 means exited)\n", why);
      wait(0);
      exit(0);
    }

Add $U/_dbgmini\ to UPROGS, then make clean; make qemu. Expected:
    $ dbgmini
    dbgmini: stopped, reason 1, pc 0x0000000000000..., sp 0x0000000000002...
    dbgmini: bytes at pc: ...
    hello from the target
    dbgmini: final reason 3 (3 means exited)

The target prints nothing until dbg_cont. If the first line says reason 1 and the last says reason 3, the kernel side works. Run it ten times, then run usertests.

Commit the reviewed source and evidence on feature/signals-debugger, push it and open the milestone PR. Suggested message: v0.12: debugger kernel support. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

Common mistakes

| Symptom | Cause |
| --- | --- |
| dbg_wait returns -1 at once | the child's parent is not the main thread, or dbg_traceme was not called before exec |
| dbg_wait hangs forever | the stop does not wakeup(p->parent), or the debugger sleeps on a different channel than l |
| the child never stops after exec | the dbg_stop call in sys_exec is missing or placed after the return |
| dbg_peek or dbg_poke returns -1 | the child is not stopped, or the page is not mapped, or the page is shared (copyout_force refuses it) |
| panic: acquire inside copyout_force | a missing release(&ptlock) on one path |
| the child is killed with unexpected scause 0x3 | the breakpoint branch is missing, or p->traced is 0 |
| the debugged program stays frozen after the debugger exits | the reparent clean-up is missing |

### 8  Milestone 13  the dbg tool

Idea. dbg is a normal user program that uses the six kernel calls from Milestone 12. It gives you an interactive prompt:

| Command | What it does |
| --- | --- |
| break FUNC (or b) | set a breakpoint at the first instruction of function FUNC |
| run (or r), continue (or c) | start or resume the program |
| bt | print the call stack with function names |
| regs | print the registers |
| info | list the breakpoints |
| quit (or q) | kill the program and leave |

Continue on feature/signals-debugger. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### 8 1 How each part works

Function names. The debugger reads the whole program file into memory (xv6 has no lseek, so it reads from start to end and then works on the memory copy). It finds the ELF section of type SYMTAB, and keeps every symbol of type FUNC with its address and size. That table turns names into addresses (for break) and addresses into names (for bt).

Setting a breakpoint. At the function's address the debugger reads 4 bytes and looks at the lowest two bits of the first byte. If they are 11 the instruction is 4 bytes long; otherwise it is a 2-byte compressed instruction. It saves those original bytes and overwrites them with ebreak (4 bytes) or c.ebreak (2 bytes). We refuse a breakpoint if that first instruction is a jump or a branch, for the reason below.

Continuing past a breakpoint. If you simply resume, the program executes the ebreak again at once. The standard way around it:

put the original instruction back at the breakpoint

plant a temporary breakpoint on the instruction right after it

resume: the program runs exactly that one original instruction and stops at the temporary breakpoint

remove the temporary breakpoint, put the real ebreak back, and resume for real

The temporary breakpoint is placed at address + length. That is only correct if the instruction we stepped over is not a jump or branch, which is why break refuses those. Function entry points normally start with a stack adjustment, so this works.

The call stack. In xv6 all code keeps frame pointers. At the first instruction of a function the new frame does not exist yet, so frame 0 is pc, frame 1 is ra (the caller), and the caller's own frame is found through fp. From there each frame holds the next return address at fp - 8 and the next frame pointer at fp - 16. We stop when an address is not inside a known function, when the frame pointer stops climbing, or after 16 frames.

#### 8 2 The source
    #include "kernel/types.h"
    #include "kernel/stat.h"
    #include "kernel/fcntl.h"
    #include "kernel/elf.h"
    #include "kernel/dbg.h"
    #include "user/user.h"

    #define MAXFUNC 256
    #define MAXBP   8

    // ELF64 section header and symbol. xv6's elf.h only has the file header.
    struct shdr {
      uint32 name;
      uint32 type;
      uint64 flags;
      uint64 addr;
      uint64 offset;
      uint64 size;
      uint32 link;
      uint32 info;
      uint64 addralign;
      uint64 entsize;
    };

    struct sym {
      uint32 name;
      uchar info;
      uchar other;
      ushort shndx;
      uint64 value;
      uint64 size;
    };

    struct func {
      uint64 addr;
      uint64 size;
      char *name;
    };

    struct bp {
      uint64 addr;
      int len;              // 2 or 4 bytes
      uchar orig[4];        // the instruction we replaced
    };

    static struct func funcs[MAXFUNC];
    static int nfuncs;
    static struct bp bps[MAXBP];
    static int nbps;
    static int pid;

    // ---- symbols ------------------------------------------------------------------

    static int
    load_symbols(char *path)
    {
      struct stat st;
      int fd = open(path, O_RDONLY);

      if(fd < 0 || fstat(fd, &st) < 0){
        printf("dbg: cannot open %s\n", path);
        return -1;
      }
      char *img = malloc(st.size);
      if(img == 0){
        printf("dbg: out of memory\n");
        return -1;
      }
      uint64 got = 0;
      int n;
      while(got < st.size && (n = read(fd, img + got, st.size - got)) > 0)
        got += n;
      close(fd);
      if(got != st.size){
        printf("dbg: short read\n");
        return -1;
      }

      struct elfhdr *eh = (struct elfhdr *)img;
      if(eh->magic != ELF_MAGIC){
        printf("dbg: %s is not an ELF file\n", path);
        return -1;
      }
      struct shdr *sh = (struct shdr *)(img + eh->shoff);
      for(int i = 0; i < eh->shnum; i++){
        if(sh[i].type != 2)                       // 2 = SHT_SYMTAB
          continue;
        struct sym *syms = (struct sym *)(img + sh[i].offset);
        int nsyms = sh[i].size / sizeof(struct sym);
        char *strtab = img + sh[sh[i].link].offset;
        for(int j = 0; j < nsyms && nfuncs < MAXFUNC; j++){
          if((syms[j].info & 0xf) != 2 || syms[j].value == 0)   // 2 = STT_FUNC
            continue;
          funcs[nfuncs].addr = syms[j].value;
          funcs[nfuncs].size = syms[j].size;
          funcs[nfuncs].name = strtab + syms[j].name;
          nfuncs++;
        }
      }
      return 0;                                    // img stays allocated: names point into it
    }

    static struct func *
    func_by_name(char *name)
    {
      for(int i = 0; i < nfuncs; i++)
        if(strcmp(funcs[i].name, name) == 0)
          return &funcs[i];
      return 0;
    }

    static struct func *
    func_by_addr(uint64 addr)
    {
      for(int i = 0; i < nfuncs; i++)
        if(addr >= funcs[i].addr && addr < funcs[i].addr + funcs[i].size)
          return &funcs[i];
      return 0;
    }

    // ---- instructions -------------------------------------------------------------

    static int
    insn_len(uchar *b)                            // b points at the first byte
    {
      return (b[0] & 3) == 3 ? 4 : 2;
    }

    // Is this a jump or a branch? We cannot step over those with the simple method.
    static int
    is_control_flow(uchar *b, int len)
    {
      if(len == 4){
        uint op = b[0] & 0x7f;
        return op == 0x63 || op == 0x6f || op == 0x67;    // branch, jal, jalr
      }
      uint h = b[0] | (b[1] << 8);
      uint quadrant = h & 3;
      uint funct3 = h >> 13;
      if(quadrant == 1 && funct3 >= 5)                    // c.j, c.beqz, c.bnez
        return 1;
      if(quadrant == 2 && funct3 == 4 && (h & 0x7c) == 0) // c.jr, c.jalr, c.ebreak
        return 1;
      return 0;
    }

    static void
    write_ebreak(uint64 addr, int len)
    {
      uchar c_ebreak[2] = { 0x02, 0x90 };               // c.ebreak = 0x9002
      uchar ebreak[4]   = { 0x73, 0x00, 0x10, 0x00 };   // ebreak   = 0x00100073
      dbg_poke(pid, addr, len == 2 ? (void*)c_ebreak : (void*)ebreak, len);
    }

    // ---- breakpoints --------------------------------------------------------------

    static struct bp *
    bp_at(uint64 addr)
    {
      for(int i = 0; i < nbps; i++)
        if(bps[i].addr == addr)
          return &bps[i];
      return 0;
    }

    static void
    set_breakpoint(char *name)
    {
      struct func *f = func_by_name(name);
      uchar b[4];

      if(f == 0){
        printf("no function named %s\n", name);
        return;
      }
      if(bp_at(f->addr)){
        printf("a breakpoint is already set at %s\n", name);
        return;
      }
      if(nbps == MAXBP){
        printf("too many breakpoints\n");
        return;
      }
      if(dbg_peek(pid, f->addr, b, 4) < 0){
        printf("cannot read the program's memory\n");
        return;
      }
      int len = insn_len(b);
      if(is_control_flow(b, len)){
        printf("cannot break at %s: its first instruction is a jump or branch\n", name);
        return;
      }
      struct bp *p = &bps[nbps++];
      p->addr = f->addr;
      p->len = len;
      memmove(p->orig, b, len);
      write_ebreak(p->addr, p->len);
      printf("breakpoint %d at %p (%s)\n", nbps, f->addr, f->name);
    }

    static void
    list_breakpoints(void)
    {
      for(int i = 0; i < nbps; i++){
        struct func *f = func_by_addr(bps[i].addr);
        printf("%d  %s  (%p)\n", i + 1, f ? f->name : "?", bps[i].addr);
      }
      if(nbps == 0)
        printf("no breakpoints\n");
    }

    // Resume the program. Returns DBG_BREAK if it stopped again, DBG_EXITED if it ended.
    static int
    resume(void)
    {
      struct dbgregs r;

      if(dbg_getregs(pid, &r) < 0)
        return DBG_EXITED;
      struct bp *b = bp_at(r.pc);
      if(b){
        // Step over the breakpoint: put the real instruction back, plant a
        // temporary breakpoint right after it, run that one instruction, then
        // put both back the way they were.
        uchar after[4];
        uint64 next = b->addr + b->len;
        dbg_poke(pid, b->addr, b->orig, b->len);
        dbg_peek(pid, next, after, 4);
        int alen = insn_len(after);
        write_ebreak(next, alen);
        dbg_cont(pid);
        int why = dbg_wait(pid);
        if(why != DBG_BREAK)
          return DBG_EXITED;
        dbg_poke(pid, next, after, alen);         // remove the temporary breakpoint
        write_ebreak(b->addr, b->len);            // restore the real breakpoint
      }
      dbg_cont(pid);
      return dbg_wait(pid);
    }

    // ---- looking at the stopped program ----------------------------------------------

    static void
    report_stop(void)
    {
      struct dbgregs r;

      dbg_getregs(pid, &r);
      struct bp *b = bp_at(r.pc);
      struct func *f = func_by_addr(r.pc);
      if(b)
        printf("Breakpoint %d, %s (%p)\n", (int)(b - bps) + 1, f ? f->name : "?", r.pc);
      else
        printf("stopped at %p\n", r.pc);
    }

    static void
    print_frame(int n, uint64 addr)
    {
      struct func *f = func_by_addr(addr);

      if(f)
        printf("#%d  %s + %d  (%p)\n", n, f->name, (int)(addr - f->addr), addr);
      else
        printf("#%d  ?  (%p)\n", n, addr);
    }

    static void
    backtrace(void)
    {
      struct dbgregs r;
      uint64 fp, floor, ra, next_fp;
      int n = 0;

      if(dbg_getregs(pid, &r) < 0)
        return;
      print_frame(n++, r.pc);
      print_frame(n++, r.ra);        // at a function's first instruction the caller is in ra
      fp = r.fp;
      floor = r.sp;
      while(n < 16){
        if(fp == 0 || fp < floor)
          break;                     // frame pointers must climb
        if(dbg_peek(pid, fp - 8, &ra, 8) < 0 || dbg_peek(pid, fp - 16, &next_fp, 8) < 0)
          break;
        if(func_by_addr(ra) == 0)
          break;                     // not code we know: end of the chain
        print_frame(n++, ra);
        floor = fp + 1;
        fp = next_fp;
      }
    }

    static void
    print_regs(void)
    {
      struct dbgregs r;

      if(dbg_getregs(pid, &r) < 0){
        printf("cannot read registers\n");
        return;
      }
      printf("pc %p  ra %p\nsp %p  fp %p\n", r.pc, r.ra, r.sp, r.fp);
      printf("a0 %p  a1 %p  a2 %p  a3 %p\n", r.a0, r.a1, r.a2, r.a3);
      printf("a4 %p  a5 %p  a6 %p  a7 %p\n", r.a4, r.a5, r.a6, r.a7);
    }

    // ---- main ---------------------------------------------------------------------

    int
    main(int argc, char *argv[])
    {
      char line[100];
      int exited = 0;

      if(argc < 2){
        printf("usage: dbg program [args]\n");
        exit(1);
      }
      if(load_symbols(argv[1]) < 0)
        exit(1);

      pid = fork();
      if(pid < 0){
        printf("dbg: fork failed\n");
        exit(1);
      }
      if(pid == 0){
        dbg_traceme();                            // let my parent control me
        exec(argv[1], &argv[1]);
        printf("dbg: exec %s failed\n", argv[1]);
        exit(1);
      }
      if(dbg_wait(pid) != DBG_EXEC){
        printf("dbg: the program did not start\n");
        exit(1);
      }
      printf("dbg: %s loaded, %d functions. Type help.\n", argv[1], nfuncs);

      for(;;){
        printf("(dbg) ");
        gets(line, sizeof(line));
        if(line[0] == 0)
          break;                                  // end of input
        line[strlen(line) - 1] = 0;               // drop the newline
        char *arg = strchr(line, ' ');
        if(arg){
          *arg++ = 0;
          while(*arg == ' ')
            arg++;
        }

        if(strcmp(line, "quit") == 0 || strcmp(line, "q") == 0)
          break;
        else if(strcmp(line, "help") == 0 || strcmp(line, "h") == 0)
          printf("break FUNC | run | continue | bt | regs | info | quit\n");
        else if(exited)
          printf("the program has exited: type quit\n");
        else if((strcmp(line, "break") == 0 || strcmp(line, "b") == 0) && arg)
          set_breakpoint(arg);
        else if(strcmp(line, "run") == 0 || strcmp(line, "r") == 0 ||
                strcmp(line, "continue") == 0 || strcmp(line, "c") == 0){
          if(resume() == DBG_BREAK){
            report_stop();
          } else {
            printf("program exited\n");
            wait(0);
            exited = 1;
          }
        }
        else if(strcmp(line, "bt") == 0)
          backtrace();
        else if(strcmp(line, "regs") == 0)
          print_regs();
        else if(strcmp(line, "info") == 0)
          list_breakpoints();
        else
          printf("unknown command: type help\n");
      }

      if(!exited){
        kill(pid);                                // the program is frozen: kill it,
        dbg_cont(pid);                            // let it notice, and collect it
        wait(0);
      }
      exit(0);
    }

Add $U/_dbg\ to UPROGS.

#### 8 3 A program to debug

The functions are marked noinline so the compiler keeps them as separate calls.
    #include "kernel/types.h"
    #include "user/user.h"

    __attribute__((noinline)) int
    inner(int x)
    {
      return x * 2;
    }

    __attribute__((noinline)) int
    middle(int x)
    {
      return inner(x) + 1;
    }

    __attribute__((noinline)) int
    outer(int x)
    {
      return middle(x) + 1;
    }

    int
    main(void)
    {
      printf("dbgtarget: outer(5) = %d\n", outer(5));
      exit(0);
    }

Add $U/_dbgtarget\ to UPROGS.

#### 8 4 Build  run  verify
    make clean
    make qemu
    $ dbg dbgtarget

A session should look like this. Addresses and offsets differ on your build:
    dbg: dbgtarget loaded, 31 functions. Type help.
    (dbg) break inner
    breakpoint 1 at 0x0000000000000a2c (inner)
    (dbg) break middle
    breakpoint 2 at 0x0000000000000a4e (middle)
    (dbg) run
    Breakpoint 2, middle (0x0000000000000a4e)
    (dbg) bt
    #0  middle + 0  (0x0000000000000a4e)
    #1  outer + 14  (0x0000000000000a7a)
    #2  main + 12  (0x0000000000000aa6)
    #3  start + 20  (0x0000000000000f30)
    (dbg) continue
    Breakpoint 1, inner (0x0000000000000a2c)
    (dbg) bt
    #0  inner + 0  (0x0000000000000a2c)
    #1  middle + 20  (0x0000000000000a6a)
    #2  outer + 14  (0x0000000000000a7a)
    #3  main + 12  (0x0000000000000aa6)
    #4  start + 20  (0x0000000000000f30)
    (dbg) continue
    dbgtarget: outer(5) = 12
    program exited
    (dbg) quit

The proof that it works:

the program stops at the right function both times

the backtrace lists every active function in the right order, and one more frame appears after continue (the call into inner)

after continue the program finishes with the correct result, 12, which shows the instruction under each breakpoint was restored and run correctly

dbg itself returns to the shell with no frozen process left (Ctrl-p shows none)

Try also regs, info, break nosuchfunction and break on the same function twice.

Commit the reviewed source and evidence on feature/signals-debugger, push it and open the milestone PR. Suggested message: v0.13: dbg tool. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

Common mistakes

| Symptom | Cause |
| --- | --- |
| dbg: cannot open or no functions found | the program file has no symbol table (it was stripped) or st.size read failed |
| garbage names | the ELF structs do not match: check field order and sizes in struct shdr and struct sym |
| the program prints usertrap(): unexpected scause 0x2 after continuing | the wrong length was used when writing or restoring the instruction (2 versus 4 bytes) |
| the same breakpoint triggers again and again | the step-over logic is missing: the original instruction was not restored before resuming |
| break works but the program never stops | the bytes were written into a shared page (copyout_force refuses, check the return value) or fence.i is missing |
| backtrace shows only one or two frames | the program was built without frame pointers, or fp is read from the wrong register (s0) |
| backtrace has extra ? frames | the stop test func_by_addr(ra) == 0 is missing |
| dbg hangs after quit | kill, dbg_cont and wait must all run, in that order |

### 9  Milestone 14  demos and integration

Now you check that all three parts of xv6-Plus work together.

#### 9 1 One program that uses everything

This program uses lazy memory, threads with a mutex, and a signal from another process. It forks the signaling child before creating threads, because fork in a multi-threaded process is not supported on several CPUs.
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"
    #include "user/sig.h"
    #include "kernel/memstat.h"

    #define NT          3
    #define PAGE        4096
    #define PER_THREAD  (2 * 1024 * 1024)       // each thread touches 2 MB

    static char *region;
    static mutex_t lock;
    static int pages_touched;
    static volatile int got_signal;

    static void
    on_usr1(int sig)
    {
      got_signal = sig;
    }

    static void
    worker(void *arg)
    {
      int id = (int)(uint64)arg;
      char *mine = region + id * PER_THREAD;

      for(int off = 0; off < PER_THREAD; off += PAGE){
        mine[off] = 1;                          // a page fault: lazy allocation
        mutex_lock(&lock);
        pages_touched++;
        mutex_unlock(&lock);
      }
    }

    int
    main(void)
    {
      int tids[NT];
      struct memstat before, after;
      int parent = getpid();

      region = sbrk(NT * PER_THREAD);           // lazy: nothing is allocated yet
      mutex_init(&lock);
      signal(SIGUSR1, (uint64)on_usr1);
      memstat(&before);

      if(fork() == 0){                          // child: signal the parent a little later
        sleep(20);
        sigsend(parent, SIGUSR1);
        exit(0);
      }

      for(int i = 0; i < NT; i++)
        tids[i] = thread_create(worker, (void*)(uint64)i);
      for(int i = 0; i < NT; i++)
        thread_join(tids[i]);

      while(!got_signal)                        // poll: sleep() returns early when a signal arrives
        sleep(1);
      wait(0);

      memstat(&after);
      printf("finaldemo: %d threads touched %d pages, signal %d received\n",
             NT, pages_touched, got_signal);
      printf("finaldemo: lazy allocations %d, page faults %d\n",
             (int)(after.lazy_allocs - before.lazy_allocs),
             (int)(after.page_faults - before.page_faults));
      exit(pages_touched == NT * (PER_THREAD / PAGE) && got_signal == SIGUSR1 ? 0 : 1);
    }

Add $U/_finaldemo\ to UPROGS. Expected (the last numbers vary):
    $ finaldemo
    finaldemo: 3 threads touched 1536 pages, signal 10 received
    finaldemo: lazy allocations 1540, page faults 1545

The pages touched must be exactly 1536 (3 threads times 512 pages), and the signal number must be 10. Try trace 2 finaldemo as well: the fork call is printed.

#### 9 2 Whole project regression

Run the full list before you tag the final release:

| Part | Programs |
| --- | --- |
| 1 Memory | baseline, lazytest, cowtest, forkbench, memstat |
| 2 Threads | kthreadtest, futextest, threadtest, racetest, prodcons, philosophers, exittest, speedup |
| 3 Signals, trace, debugger | trace 32 cat README, sigtest, dbgmini, dbg dbgtarget, finaldemo |
| everything | usertests |
| swap (CPUS=1 RAM_MB=8) | memhog 12, swapfork, hotcold |

Commit the reviewed source and evidence on feature/signals-debugger, push it and open the milestone PR. Suggested message: v1.0: xv6-Plus. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

### 10  Final test and demo checklist

| Test | Command | Expected |
| --- | --- | --- |
| Trace | trace 32 cat README | one syscall read line per read, the last one returning 0 |
| Trace follows children | trace 2 sh, then run ls | syscall fork lines appear |
| Signals | sigtest | five ok lines and sigtest: ALL OK |
| Debugger kernel calls | dbgmini | reason 1, then reason 3; the target prints its text between them |
| Debugger tool | dbg dbgtarget with break inner, break middle, run, bt, continue | stops in the right functions, correct backtrace, result 12 |
| Debugger clean-up | quit dbg while the program is frozen, then press Ctrl-p | no frozen process is left |
| Integration | finaldemo | 1536 pages touched, signal 10 received |
| Earlier parts | the regression table in section 9.2 | all pass |
| Whole system | usertests | ALL TESTS PASSED |
| Leaks | memstat before and after each program | Compare repeated equivalent states; investigate persistent growth and explain the saved base residual slot. |

Run every program at least ten times. Signal and thread bugs are timing bugs, so a single clean run proves little.

Demo script for Part 3 (about 4 minutes):

trace 32 cat README: "every system call a program makes is visible".

sigtest: self signal, signal to another process, default action, ignored signal, signal interrupting sleep.

dbg dbgtarget: set two breakpoints, run, show the backtrace growing, continue to the correct result.

finaldemo: lazy memory, threads, a mutex and a signal in one program.

### 11  Report outline and viva questions

Report outline for Part 3: (1) motivation: why signals, tracing and a debugger; (2) background: signals, traps, ebreak, frame pointers, ELF symbols; (3) design: delivery path, pending mask, trap-based stop, debugger protocol; (4) implementation: files changed (git diff --stat v0.9..v1.0); (5) results: test outputs, the sample debugger session, the leak check; (6) limitations and future work; (7) conclusion.

| Question | Short answer |
| --- | --- |
| Why is a signal not delivered the moment it is sent? | The kernel never interrupts its own work. It sets a pending bit and delivers on the next return to user mode |
| What does sigreturn do? | It copies the saved registers back, so the program continues exactly where it was interrupted |
| Why save the whole trapframe? | The handler may change any register; the interrupted code expects all of them back |
| Why must sigreturn return the restored a0? | syscall() writes the return value into a0 afterwards; returning anything else would corrupt the program's a0 |
| Why can SIGKILL not be caught? | A process that ignores it could never be stopped, so the kernel applies it directly |
| Why does only the main thread run handlers? | Signal state lives in the process; running a handler on an arbitrary thread would need per-thread masks and stacks, which we did not build |
| What is the race in pause() and how do you avoid it? | A signal can arrive between checking a flag and calling pause. Poll with sleep(1) instead, or sleep under the same lock the sender takes |
| How does ebreak stop a program? | It raises a breakpoint exception (cause 3). The kernel sees a traced process, freezes it and wakes the debugger |
| Why two breakpoint sizes? | RISC-V has 2-byte compressed and 4-byte instructions. We overwrite exactly one instruction, so the sizes must match |
| How do you continue from a breakpoint? | Restore the original instruction, plant a temporary breakpoint after it, run that one instruction, then put the breakpoint back |
| Why only breakpoints at function entry? | The step-over method assumes the next instruction is at pc + length, which is false for jumps and branches |
| How does the backtrace work? | Each frame stores the return address at fp - 8 and the caller's frame pointer at fp - 16; we follow the chain and look up names in the ELF symbol table |
| Why is copyout_force needed? | Code pages are read-only, so ordinary copyout refuses to write them |
| What happens if the debugger exits first? | reparent finds the traced child, clears its stop state and kills it, so nothing stays frozen |

### 12  Troubleshooting reference

| Symptom | Likely cause | What to check |
| --- | --- | --- |
| kernel does not boot after the syscall.c edit | table or name array indexes out of range, or a missing extern | number of entries, NELEM(syscalls) |
| trace prints nothing | mask not copied by fork, or the check uses 1 << num with a large num | num < 31 guard |
| handler never runs | deliver_signals not called, or the process is not the main thread | usertrap end, p == p->leader |
| crash after a handler returns | restorer address missing, or sigreturn does not restore the full trapframe | sig.c, sys_sigreturn |
| a later value in the program is wrong after a signal | sys_sigreturn returned 0 | return p->trapframe->a0 |
| pause() or sleep ignores signals | missing wakeup(p) in sigsend, or signal_pending not checked in sys_sleep | signal.c, sysproc.c |
| dbg_wait hangs | stop does not wake the parent, or channels differ | wakeup(p->parent) and sleep(l, ...) |
| target never stops after exec | stop missing in sys_exec, or dbg_traceme not called first | sysfile.c |
| target dies with unexpected scause 0x3 | no breakpoint branch, or p->traced is 0 | usertrap |
| target dies with unexpected scause 0x2 after continue | wrong breakpoint length, or step-over missing | write_ebreak, resume |
| dbg_poke fails | page shared, not resident, or the target is not stopped | copyout_force, stopped_target |
| backtrace has only two frames | frame pointers missing, or fp read from the wrong register | s0 in sys_dbg_getregs |
| panic: sched locks | dbg_stop or sys_pause slept while holding another spinlock | lock order |
| a frozen child after the debugger quits | reparent clean-up missing | proc.c |

A general recipe: reproduce with the smallest program, add printf lines in the kernel path (stop, signal delivery), then use GDB with a breakpoint at usertrap or deliver_signals and inspect p->trapframe->epc.

### 13  Known limitations

Signals are delivered only to the main thread, there is no signal mask, a signal during a handler waits until the handler returns, and only sleep and pause are interrupted. A blocked read or wait is not.

Console Ctrl-C is not wired to SIGINT. xv6 has no notion of a foreground process.

trace covers system calls 1 to 30 only, because the mask is an int.

The debugger works on one single-threaded program at a time. Breakpoints go on function entry points only. It has no single-step, no watchpoints and no variable printing.

Shared code pages cannot be modified by dbg_poke. The program being debugged must have been started by exec from dbg itself.

Frame 1 of a backtrace is correct only when the program stopped at a function's first instruction.

fence.i is executed locally; a debugged program that moves between CPUs relies on the fence.i in usertrapret.

### 14  Putting the whole project together

#### 14 1 Repository and README checklist

| Item | Content |
| --- | --- |
| Tags | v0.1 through v1.0, one per milestone, so a reader can see the project grow |
| README.md | what xv6-Plus is, a feature table (the three parts), build and run commands (make qemu, CPUS=1 RAM_MB=8 for swap), the list of test programs and what each proves |
| docs/ | one short design note per part: page-fault handler, thread design with the trapframe trick, signal and debugger protocol |
| user/ | all test and demo programs |
| Build check | a fresh clone must build and boot with make qemu and no manual steps |
| Cleanup | no leftover debug printf in the kernel; every // TODO resolved or listed under limitations |

#### 14 2 Final report outline

Introduction and motivation (why xv6, why these features)

Background (virtual memory, threads and races, traps and signals)

Memory management: lazy allocation, copy-on-write, swap, memstat

Threads: shared address space, per-thread trapframe, futex, library

Signals, tracing and the debugger

Evaluation: baseline versus lazy sbrk, forkbench ticks, swap statistics and the policy table, the race demo, the speedup table, the leak checks

Limitations and future work

Conclusion, with the team's contribution table

Appendix: build instructions, tag list, git log --oneline

#### 14 3 The final demo

| Time | What to show |
| --- | --- |
| 0:00 | one slide: stock xv6 versus xv6-Plus (the comparison table from the deck) |
| 0:30 | Memory: baseline and lazytest (200 MB on a 128 MB machine), cowtest and forkbench, then boot with RAM_MB=8 and run memhog 12 with memstat |
| 3:00 | Threads: racetest (lost updates against the mutex), prodcons, speedup 1 against speedup 3, exittest |
| 5:00 | Signals and trace: trace 32 cat README, sigtest |
| 6:00 | Debugger: the dbg dbgtarget session with the backtrace |
| 7:30 | finaldemo, then the closing slide with limitations and future work |

Record it once with no cuts, then re-record only the sections that go wrong.

#### 14 4 The two minute pitch

Final presentation script template after all milestones pass: Explain the memory features, thread model, synchronization, signals, tracing and debugger in that order. Use your own measured results, then state the one CPU swap limit, main thread signals and single threaded debugger target limit. Do not present unfinished features as completed work.

#### 14 5 Resume bullets

xv6-Plus: Extended Operating System Kernel | C, RISC-V assembly, QEMU, GDB, Git

Possible final resume wording after validation: Extended xv6 with lazy allocation, COW fork, memory statistics and FIFO and Clock swap; report the exact benchmark workload, configuration and recorded measurements.

Possible final resume wording after validation: Built kernel threads, futex wait and wake and a user synchronization library; describe the measured speedup and race correction only after obtaining the results.

Implemented signals with handlers and sigreturn, a trace system call, and an ebreak-based debugger with ELF-symbol call stacks

Describe the actual completed release history at submission. Include only implemented and tested milestones, their tags, the report and recorded demonstration.

#### 14 6 Ideas if you still have time

| Extension | Why it is interesting |
| --- | --- |
| Wire console Ctrl-C to SIGINT | the signal system finally becomes visible to a user at the keyboard |
| alarm(ticks) and SIGALRM | a timer-driven signal; builds on sigsend and the timer interrupt |
| step, x and finish commands in dbg | single-stepping and memory examination |
| A thread-safe malloc | removes the biggest limitation of the thread library |
| A shared file descriptor table | threads then see files opened by other threads |
| TLB shootdown and multi-CPU swap | the real fix for the one-CPU limit on swap |
| mmap | file-backed memory using the page-fault machinery from Part 1 |

#### 14 7 Before you submit

☐ A fresh clone builds and boots

☐ Every regression program passes (section 9.2), including the swap tests with CPUS=1 RAM_MB=8

☐ usertests prints ALL TESTS PASSED

☐ Every release v0.1 to v1.0 is tagged

☐ README, design notes, report and demo video are done

☐ Every team member can explain the page-fault handler, the trapframe trick, the futex check-and-sleep, and the breakpoint step-over

That last line is the most important one: the project proves your skill only if each of you can explain any part of it when asked.
