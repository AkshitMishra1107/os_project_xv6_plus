
# Akshit Threads and Synchronization Implementation Guide

Two person xv6 Plus project   27 October 2026

This is the working manual for Akshit. Follow the shared GitHub routine first, then the complete implementation steps and source listings that follow. Akshit owns threads and synchronization. The teammate owns tracing, signals and the debugger. Both review shared kernel changes and validate the final integration.

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

Keep your existing source checkout and its history. In Ubuntu run these commands after the repository bootstrap has finished. If status shows edits, commit the intended work before switching branches.
    cd "/mnt/c/Users/akshi/OneDrive/Documents/TIET/Sem-5/Mini Project/xv6-plus"
    git status --short
    git fetch origin --tags
    git switch feature/threads-sync
    git pull --ff-only
    git branch --show-current

Expected branch: feature/threads-sync. Your local author is already configured as Akshit. Check the email belongs to your GitHub account before making new commits. Do not rewrite the earlier memory commits to change attribution.
    git config user.name
    git config user.email
    git remote -v
    git rev-parse v0.4

Check your own versions and record them. The saved owner environment is in docs/memory/evidence/environment-versions.txt; it does not describe the teammate machine.
    git --version
    riscv64-linux-gnu-gcc --version
    qemu-system-riscv64 --version
    gdb-multiarch --version

## Daily edit commit and push routine

Start with a clean working tree on your own branch. Fetch main and merge it before beginning a new chunk. Do not rebase or force push these shared history branches.
    git switch feature/threads-sync
    git status --short
    git fetch origin
    git merge origin/main

Implement one small logical change. Build and run its focused tests. Save the exact configuration, output and relevant screenshots. Then review all changes and stage the intended source and your evidence directory. The existing ignore rules keep the usual build products out of Git; still inspect the staged list.
    git status --short
    git diff --check
    git diff
    git add kernel user Makefile docs/threads
    git diff --cached --stat
    git diff --cached --check

Use a descriptive message for the actual completed change. For example:
    git commit -m "v0.5: prepare memory and page faults for threads"
    git push origin feature/threads-sync

Several small commits may belong to one milestone PR. Keep incomplete work on your feature branch and push it for backup; describe what is incomplete in the draft PR. Do not tag an unfinished version. A rejected push means fetch and reconcile the branch first; it is not a reason to force push.

## Pull requests review and release tags

On GitHub open Pull requests and New pull request. Set base to main and compare to your own feature branch. Use the milestone as the title. Describe the problem and behavior, shared files changed, exact build options, tests actually run and remaining limits. Use a draft PR while dependencies or checks are incomplete. The other person reviews and reproduces the important checks before a ready PR is merged.

Merge with Create a merge commit. This preserves the individual feature commits and an explicit integration point. Keep both long lived feature branches after each merge so the next milestone continues from the same branch. In the owner role, do not change repository merge policy without first agreeing it with your teammate.

After a PR is merged, update your feature branch and continue. The main version must pass its milestone checks before the integrator creates its official tag. Only Akshit creates release tags, after both people agree that the checks are complete.
    git fetch origin
    git switch feature/threads-sync
    git merge origin/main
    git push origin feature/threads-sync

Release procedure example for v0.5. Run this only when the v0.5 PR is merged and the integrated commit has been tested. Check that the tag does not already exist; never replace an existing release tag.
    git fetch origin --tags
    git switch main
    git pull --ff-only
    git status --short
    git tag --list v0.5
    # After the integrated tests pass and no v0.5 tag exists
    git tag -a v0.5 -m "v0.5 thread ready memory validated"
    git push origin refs/tags/v0.5

Use the correct version for each later milestone, then return to feature/threads-sync. Record the tag commit ID, PR URL and configuration in the release notes. Do not run the example v0.5 tag command while working on a different version.

## Milestone ownership and commit messages

| Release | Deliverable | Suggested milestone commit |
| --- | --- | --- |
| v0.5 | Thread ready memory and locking | v0.5: prepare memory and page faults for threads |
| v0.6 | Kernel threads and process lifecycle | v0.6: add kernel threads and lifecycle handling |
| v0.7 | Futex wait and wake | v0.7: add futex wait and wake |
| v0.8 | User thread and synchronization library | v0.8: add thread mutex condition and semaphore library |
| v0.9 | Thread demonstrations and regression | v0.9: add and validate thread demonstrations |

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

A terminal transcript can be captured from Ubuntu with script -q docs/threads/session.txt, followed by the build and xv6 run. After leaving QEMU, type exit to close the capture. Use a new descriptive filename for each milestone so earlier evidence is preserved.
    script -q docs/threads/v05-session.txt
    # Run the relevant build and test session
    # After Ctrl A then X exits QEMU
    exit

Change v05 in that example to the actual milestone or prototype name. Before committing a transcript, remove accidental credentials if any were typed. Record the real outputs; never paste the expected output in place of an observed test.

For every milestone save docs/threads/release-notes.md, transcripts and screenshots. Record date, author, branch, commit, CPU count, RAM_MB, SWAP_POLICY, exact test commands, observed results, repeat counts, failures and fixes. Include before and after memstat and explain any persistent difference. A single residual swapped page already occurred in the memory base; it was stable across repeats, and its owner was not traced. Compare repeated equivalent states rather than declaring every nonzero slot a new leak.

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

## Akshit first implementation task

Begin at Milestone 5 in the implementation chapters. First reproduce the saved memory tests in the one CPU 128 MB mode. Then inspect all existing interrupt guards before introducing ptlock. The supplied guide predates some v0.4 repairs, so its mechanical replacement table is incomplete for this checkout.
    rg -n "push_off|pop_off|pin_user_page|uvmunmap" kernel/vm.c kernel/swap.c kernel/pgfault.c
    rg -n "swap_track|swap_untrack|kfree" kernel/kalloc.c kernel/vm.c kernel/swap.c
    rg -n "copyin|copyout" kernel/pipe.c
    rg -n "freeproc|copyout|xstate" kernel/proc.c

Create a lock ownership table in docs/threads/release-notes.md before editing. For each function record whether its caller already holds ptlock, whether it may sleep and what state it returns with. The v0.4 swap_track, swap_untrack, swap_untrack_owner and uvmunmap already contain push_off and pop_off pairs. A blind replacement can make a caller take ptlock twice and panic with acquire.

Do not make kfree unconditionally acquire ptlock through swap_untrack. kfree also runs during boot in kinit before swap_init initializes ptlock, and it is called from paths that may already hold ptlock. Frame bookkeeping needs a deliberate initialization and locking contract, such as documented internal helpers for callers already holding the lock. Review that contract before choosing the implementation; no universal search and replace is safe here.

Also preserve the v0.4 wait cleanup ordering: capture child status, release its memory and locks, then copy status to the parent. Carry the intent into thread aware wait and join. Reintroducing copyout under wait or child locks can break low memory behavior. Keep the pipewrite kernel staging buffer and the piperead user copy outside the pipe lock.

## Part 2 acceptance gates

| Version | Required work | Checks before its PR is ready |
| --- | --- | --- |
| v0.5 | Constants leader fields ptlock fault access types swap in recheck shared size and call sites | Part 1 demonstrations usertests including sbrkfail and pipe1 then one CPU pressure checks under both policies. |
| v0.6 | Per thread trapframes create exit join gettid and process lifecycle | kthreadtest repeated; thread exit join and process exit; earlier usertests and memory checks. |
| v0.7 | Futex expected value check and sleep under one lock plus wake | futextest repeated and no lost wakeup; reject invalid addresses according to the implemented API. |
| v0.8 | Thread library mutex condition variable semaphore and linking | threadtest repeated with correct synchronized counts; allocate shared buffers before starting workers. |
| v0.9 | Race producer consumer philosophers exit and speedup demos | All Part 2 programs at least ten runs; speedup 1 2 and 3 with repeated ticks; full usertests and prior memory regressions. |

Run speedup configurations with the same workload and build. Record several runs and report a summary such as the median; do not promise a three times speedup. An unsynchronized race may occasionally obtain the expected count, so repeat it and show the mutex version consistently produces the exact count. After validated v0.9 is merged and tagged, notify your teammate through your normal communication channel that the Part 3 base is ready.

## Implementation chapters for threads and synchronization

Complete concepts, source listings, tests and troubleshooting adapted from the supplied Part 2 guide. The shared workflow and compatibility notes above take precedence over the original team schedule and release commands. Implement and validate each milestone on your assigned feature branch.

### 1  What you will build

Part 2 adds threads to xv6. A thread is another flow of execution inside the same program: it shares the program's memory but has its own registers and stacks. Each release works on its own.

| Release | Feature | What it gives you |
| --- | --- | --- |
| v0.5 | Thread-ready memory code | Part 1's page-fault code stays correct when several threads share one address space |
| v0.6 | Kernel threads | kthread_create, kthread_exit, kthread_join; threads share memory and run on several CPUs at once |
| v0.7 | futex_wait and futex_wake | the kernel service every lock needs in order to sleep and wake up |
| v0.8 | Thread library | thread_create, thread_join, mutex, condition variable, semaphore |
| v0.9 | Demos and tests | lost-update race, producer-consumer, dining philosophers, exit from a thread |

Suggested implementation sequence

| Stage | Work |
| --- | --- |
| 1 | Read this guide, study the Part 1 code paths, do Milestone 5 (v0.5) |
| 2 | Thread creation, exit and join (first half of v0.6) |
| 3 | Make exit, wait, kill, fork, exec thread-aware; debug (v0.6) |
| 4 | futex and the mutex (v0.7, start of v0.8) |
| 5 | Condition variable, semaphore, demos (v0.8, v0.9) |
| 6 | Testing, report, demo video |

Akshit implements all Part 2 milestones. The teammate implements Part 3. Both review kernel interfaces, run integration checks and contribute their report sections. Work by acceptance gate rather than the original three person six week schedule.

### 2  Before you start

Part 1 must be finished and tagged v0.4. This guide changes some functions you wrote in Part 1.

Continue on feature/threads-sync. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

Tools are the same as in Part 1 (QEMU, RISC-V toolchain, GDB).

How to run. Thread demos use the default make qemu (3 CPUs, 128 MB), so threads really run in parallel. Swap demos from Part 1 still need CPUS=1 RAM_MB=8. Always make clean when you change CPUS, RAM_MB or SWAP_POLICY.

As in Part 1, function signatures differ between xv6 versions. Open the real function before editing and keep your version's style (argint returning void or int, killed(p) or p->killed).

### 3  Concepts you need

#### 3 1 Process versus thread

|  | Process | Thread |
| --- | --- | --- |
| Memory | its own private address space | shares the address space of its process |
| Registers and stack | own | own |
| Creating one | fork() copies memory (cheap with COW) | no copying at all |
| Sharing data | pipes, files | just read and write the same variables |

Stock xv6 has only processes. Every struct proc owns one page table and exactly one flow of execution.

#### 3 2 The race condition

Two threads run counter = counter + 1. That one line is really three steps: read, add, write.

| Step | Thread 1 | Thread 2 | counter |
| --- | --- | --- | --- |
| 1 | reads 5 |  | 5 |
| 2 |  | reads 5 | 5 |
| 3 | writes 6 |  | 6 |
| 4 |  | writes 6 | 6 |

Two increments happened but the counter went up by one. This lost update is a race condition.

#### 3 3 Mutex and atomic instructions

A mutex lets only one thread into a critical section at a time. It needs an atomic instruction, one the CPU performs as a single indivisible step:

compare-and-swap (CAS): "if the value is X, set it to Y", and tell me if it worked

atomic exchange: set a new value and return the old one

In C we use GCC's __sync_val_compare_and_swap, __sync_lock_test_and_set and __sync_fetch_and_add (xv6's own spinlocks use the same builtins). __sync_synchronize() is a memory barrier that stops the CPU from reordering memory accesses across it.

#### 3 4 Why the kernel must help  futex

A thread that finds the mutex taken could loop and keep checking, but that wastes the CPU. It is better to sleep until the owner unlocks, and only the kernel can put a thread to sleep. The futex idea needs just two system calls:

futex_wait(addr, expected): if the number at addr still equals expected, sleep; otherwise return at once

futex_wake(addr): wake the threads sleeping on addr

The value check and the sleep must happen atomically inside the kernel. Otherwise the unlock and wake could slip in between them and the waiter would sleep forever (a lost wakeup).

#### 3 5 Deadlock

Thread A holds lock 1 and waits for lock 2. Thread B holds lock 2 and waits for lock 1. Both wait forever. The standard fix is a lock order: every thread takes locks in the same order. The dining philosophers demo shows this.

### 4  Design  how a thread fits into xv6

Core idea. A thread is another entry in xv6's process table (struct proc) that points at the same page table as the thread that created it. The scheduler needs no change at all: it sees one more runnable entry. That is why a blocked thread does not stop the others, and why threads run on several CPUs.
    thread 0 (main)  struct proc --\
    thread 1         struct proc ---+--> one page table --> shared memory
    thread 2         struct proc --/
       each has its own: registers (trapframe), user stack, kernel stack, state

| Item | Shared by all threads | Private to each thread |
| --- | --- | --- |
| Page table and memory (code, data, heap) | yes |  |
| Memory size sz | yes (kept in the main thread's proc) |  |
| Process id | yes (threads share the pid) |  |
| Open files | no: copied when the thread is created (limitation) | the copy |
| Thread number tid |  | yes |
| Saved registers (trapframe) |  | yes |
| User stack |  | yes |
| Kernel stack |  | yes |
| Scheduler state |  | yes |

Terminology. The main thread (also called the leader) is the first thread of a process, tid 0. Every proc has a leader pointer: the main thread points to itself.

The trapframe problem. In stock xv6 the page where the CPU saves user registers on a trap is mapped at one fixed address, TRAPFRAME, and the assembly in trampoline.S hard-codes that address. Threads share a page table, so they cannot all use one address. Our fix: thread t gets its trapframe mapped at TRAPFRAME - t * PGSIZE, and the trampoline reads the address from the sscratch register instead of a constant.

Lifecycle rules

| Event | What happens |
| --- | --- |
| thread function returns | a stub calls kthread_exit; the thread ends |
| a secondary thread calls exit() | the whole process ends |
| the main thread exits | other threads are killed and reaped first, then the process ends |
| kill(pid) | marks every thread of that process (they share the pid) |
| fork() in a thread | the child is a normal single-threaded process |
| exec() | refused in a multi-threaded process |
| sbrk() | the size is shared; updates are protected by a lock |
| wait() | any thread of the parent can wait for its children |

Files you will touch

| File | Change |
| --- | --- |
| kernel/param.h, kernel/memlayout.h | thread limits and trapframe addresses |
| kernel/proc.h, kernel/proc.c | new fields, thread create, exit, join, and thread-aware exit, wait, kill, fork, growproc |
| kernel/trampoline.S, kernel/trap.c | per-thread trapframe address |
| kernel/vm.c, kernel/pgfault.c, kernel/swap.c | thread-safe locking for the Part 1 code |
| kernel/exec.c, kernel/sysproc.c, kernel/syscall.* | exec guard, thread system calls |
| kernel/futex.c (new) | futex_wait, futex_wake |
| user/thread.c, user/thread.h (new) | the user library |
| user/*.c (new) | demo programs |

### 5  Milestone 5  prepare the kernel for threads

Why. In Part 1, one process meant one flow of execution, so only one CPU could touch a process's page table at a time. With threads, two CPUs can fault on the same page at the same moment. Both would try to allocate it, both would write the same page-table entry, and the kernel would panic with remap. We fix it with one lock around page-table changes, and by making the fault handler re-check the page-table entry after any sleep.

#### Step 1  constants

In param.h:
    #define MAXTHREADS  8     // threads per process, main thread included

In memlayout.h, after the TRAPFRAME line:
    // Thread t (0 = main thread) has its trapframe mapped at TFVA(t).
    #define TFVA(t)   (TRAPFRAME - (uint64)(t) * PGSIZE)
    // User memory must stay below the lowest trapframe address.
    #define USERLIMIT TFVA(MAXTHREADS - 1)

#### Step 2  new per process fields

Add to struct proc, next to name:
      struct proc *leader;         // main thread of this process (itself for the main thread)
      int tid;                     // thread number: 0 for the main thread
      int nthreads;                // main thread only: live threads, itself included
      uint tidmask;                // main thread only: bit t set = thread number t is in use
      uint64 tf_va;                // user address where this thread's trapframe is mapped

In kernel/proc.c, in allocproc(), right after p->state = USED; add:
      p->leader = p;
      p->tid = 0;
      p->nthreads = 1;
      p->tidmask = 1;              // thread number 0 is the main thread
      p->tf_va = TRAPFRAME;

In freeproc(), add before p->state = UNUSED;:
      p->leader = 0;
      p->tid = 0;
      p->nthreads = 0;
      p->tidmask = 0;
      p->tf_va = 0;

#### Step 3  the address space lock

In kernel/vm.c (near the top, after the includes) define the lock:
    // Protects page-table changes and the thread bookkeeping of a process.
    // One global lock keeps things simple.
    struct spinlock ptlock;

In kernel/swap.c, make swap_init initialize it:
    void
    swap_init(void)
    {
      initsleeplock(&swap_lock, "swap");
      initlock(&ptlock, "ptlock");
    }

In kernel/defs.h add extern struct spinlock ptlock;, and the access types used in Step 5:
    #define ACC_READ  0
    #define ACC_WRITE 1
    #define ACC_EXEC  2

Lock order (never take them the other way round): ptlock, then pgref.lock, then kmem.lock. Do not call anything that sleeps while holding ptlock.

#### Step 4  replace push off with the lock

Use this table as the starting call site checklist, then reconcile it with the actual v0.4 wrappers described above. The additional swap tracking, unmapping, kfree boot path and pipe repair make an unrestricted mechanical replacement unsafe. Assign lock ownership once per operation and do not sleep while holding ptlock.

| Function (file) | Change |
| --- | --- |
| pin_user_page (vm.c) | push_off() becomes acquire(&ptlock). The pop_off() before calling the handler becomes release(&ptlock). On success it now returns with ptlock held |
| copyout, copyin, copyinstr (vm.c) | the pop_off() after the copy becomes release(&ptlock) |
| prefault (vm.c) | pop_off() becomes release(&ptlock) |
| uvmcopy (vm.c) | every push_off() and pop_off() becomes acquire(&ptlock) and release(&ptlock) |
| swap_out_one (swap.c) | the push_off() and pop_off() pair becomes acquire(&ptlock) and release(&ptlock) |
| handle_page_fault (pgfault.c) | rewritten in Step 5 |

A spinlock disables interrupts while held, so everything you relied on before still holds.

#### Step 5  the thread safe fault handler

Two changes from the Part 1 version:

It takes an access type (read, write or execute) instead of a yes/no write flag. When another thread has already fixed the page, the handler must know whether the access is now allowed, and answer 0 (retry) instead of killing the thread.

It uses ptlock, and the memory size comes from the main thread (p->leader->sz).

Replace handle_page_fault with:
    int
    handle_page_fault(struct proc *p, uint64 va, int access)
    {
      pagetable_t pt = p->pagetable;
      int is_write = (access == ACC_WRITE);
      char *mem = 0;                       // a spare page, allocated when needed

      stat_faults++;
      if(va >= p->leader->sz)
        return -1;
      va = PGROUNDDOWN(va);

      for(;;){
        acquire(&ptlock);                  // nobody may change this entry meanwhile
        pte_t *pte = walk(pt, va, 0);
        int lazy    = (pte == 0) || ((*pte & (PTE_V | PTE_SWAP)) == 0);
        int swapped = (pte != 0) && (*pte & PTE_SWAP);
        int cow     = (pte != 0) && (*pte & PTE_V) && is_write && (*pte & PTE_COW);
        int copy    = cow && kref_get((void*)PTE2PA(*pte)) > 1;

        if((lazy || swapped || copy) && mem == 0){
          // We need a fresh page. Getting one may sleep (swap-out), so release
          // the lock, allocate, then look at the entry again.
          release(&ptlock);
          if(swapped && nlocks_held() > 0)
            return -1;                     // a swap-in must sleep: not allowed with a spinlock held
          mem = kalloc_user();
          if(mem == 0)
            return -1;
          continue;
        }

        if(swapped){                       // case 1: bring the page back from disk
          release(&ptlock);
          swap_in(pt, va, pte, mem);       // sleeps while the disk is read
          return 0;
        }

        if(lazy){                          // case 2: first touch, give a zeroed page
          memset(mem, 0, PGSIZE);
          if(mappages(pt, va, PGSIZE, (uint64)mem,
                      PTE_R | PTE_W | PTE_U) != 0){
            release(&ptlock);
            kfree(mem);
            return -1;
          }
          swap_track((uint64)mem, pt, va);
          stat_lazy++;
          release(&ptlock);
          return 0;
        }

        if(cow){                           // case 3: store to a copy-on-write page
          uint64 pa = PTE2PA(*pte);
          if(kref_get((void*)pa) == 1){
            // we are the only owner now: make the page writable again
            *pte = (*pte & ~PTE_COW) | PTE_W;
            swap_track(pa, pt, va);
            sfence_vma();
            release(&ptlock);
            if(mem)
              kfree(mem);
            return 0;
          }
          memmove(mem, (char*)pa, PGSIZE);
          uint64 flags = (*pte & (PTE_R | PTE_X | PTE_U)) | PTE_W | PTE_V;
          *pte = PA2PTE(mem) | flags;      // this thread's process now has a private copy
          sfence_vma();
          swap_track((uint64)mem, pt, va);
          kfree((void*)pa);
          stat_cow++;
          release(&ptlock);
          return 0;
        }

        // Nothing to fix. Either another thread fixed this page a moment ago
        // (the retry will succeed), or the access really is illegal.
        int ok = pte && (*pte & PTE_V) && (*pte & PTE_U) &&
                 (access == ACC_WRITE ? (*pte & PTE_W) :
                  access == ACC_EXEC  ? (*pte & PTE_X) : (*pte & PTE_R));
        release(&ptlock);
        if(mem)
          kfree(mem);
        return ok ? 0 : -1;
      }
    }

Before, the last line returned -1 for any page that was already valid. With two threads that would wrongly kill the second thread of a lazy fault.

#### Step 6  pass the access type at every call site

kernel/trap.c, in usertrap(): replace the page-fault branch from Part 1 with:
      } else if(r_scause() == 12 || r_scause() == 13 || r_scause() == 15){
        int access = (r_scause() == 15) ? ACC_WRITE :
                     (r_scause() == 12) ? ACC_EXEC : ACC_READ;
        if(handle_page_fault(p, r_stval(), access) < 0)
          setkilled(p);                 // older xv6: p->killed = 1;
      } else if((which_dev = devintr()) != 0){

kernel/vm.c, in pin_user_page: change the handler call to
        if(pagetable != p->pagetable ||
           handle_page_fault(p, va, write ? ACC_WRITE : ACC_READ) < 0)
          return 0;

kernel/vm.c, in uvmcopy: change the swap-in call to handle_page_fault(myproc(), i, ACC_READ).

Update the prototype in defs.h: int handle_page_fault(struct proc*, uint64, int); (the third argument is now an access type).

#### Step 7  swap in must re check

Two threads can fault on the same swapped page. Both allocate a page; the first reads the data from disk; the second must notice that the work is already done instead of reading a slot that was just freed. Replace swap_in:
    void
    swap_in(pagetable_t pt, uint64 va, pte_t *pte, void *mem)
    {
      acquiresleep(&swap_lock);                     // waits for any swap operation in progress
      if((*pte & PTE_SWAP) == 0){
        // another thread of this process already brought the page back
        releasesleep(&swap_lock);
        kfree(mem);
        return;
      }
      int slot = PTE2SLOT(*pte);
      uint64 perm = *pte & (PTE_R | PTE_W | PTE_X | PTE_U);

      swap_read(slot, (uint64)mem);
      *pte = PA2PTE(mem) | perm | PTE_V;
      swap_slot_free(slot);
      releasesleep(&swap_lock);

      sfence_vma();
      swap_track((uint64)mem, pt, va);
      stat_swapin++;
    }

#### Step 8  the memory size belongs to the process

All threads must agree on the size of the address space, so we keep it only in the main thread's proc (p->leader->sz). Two threads may call sbrk at once, so the update is protected by ptlock.

Replace sys_sbrk:
    uint64
    sys_sbrk(void)
    {
      uint64 addr;
      int n;
      struct proc *l = myproc()->leader;

      argint(0, &n);
      acquire(&ptlock);                     // threads may call sbrk at the same time
      addr = l->sz;
      if(n >= 0){
        if(addr + n < addr || addr + n > USERLIMIT){
          release(&ptlock);
          return -1;
        }
        l->sz += n;                         // lazy: only record the new size
      } else if(growproc(n) < 0){           // shrinking frees pages right away
        release(&ptlock);
        return -1;
      }
      release(&ptlock);
      return addr;
    }

Replace growproc in proc.c:
    int
    growproc(int n)
    {
      uint64 sz;
      struct proc *p = myproc()->leader;    // the size and page table belong to the process

      sz = p->sz;
      if(n > 0){
        if((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0)
          return -1;
      } else if(n < 0){
        sz = uvmdealloc(p->pagetable, sz, sz + n);
      }
      p->sz = sz;
      return 0;
    }

Keep your version's uvmalloc argument list.

#### Step 9  build  test  tag

Nothing visible changes yet: this milestone must not break Part 1.
    make clean
    make qemu

| Test | Expected |
| --- | --- |
| lazytest, cowtest, forkbench, baseline | same results as before |
| usertests | ALL TESTS PASSED including sbrkfail and pipe1 |

Then rebuild the one CPU 8 MB pressure configuration under both FIFO and Clock. memhog 12 must report 0 bad and swapfork must print OK. After reviewing source and evidence, commit and open the v0.5 PR. Tag only the validated integrated main commit.
    git add kernel user Makefile docs/threads
    git diff --cached --check
    git diff --cached --stat
    git commit -m "v0.5: thread-ready memory code"
    git push origin feature/threads-sync
    # Open the milestone PR then follow the shared review routine

Common mistakes

| Symptom | Cause |
| --- | --- |
| panic: acquire | a function takes ptlock while it already holds it. Check that pin_user_page callers release it before calling anything that takes it again |
| hang right after boot | ptlock was not initialized (swap_init must call initlock) |
| compile error about ACC_WRITE | the three #defines must be in defs.h, before the prototype |
| usertrap(): unexpected scause 0xd | the handler returned -1; print stval and the access type |

### 6  Milestone 6  kernel threads

Idea. kthread_create makes a new proc entry that uses the same page table, with its own trapframe, stacks and registers. The new thread starts at the function you give it. When that function returns, a small user stub calls kthread_exit.

Continue on feature/threads-sync. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### Step 1  a trapframe address for each thread

The trampoline is assembly code that runs on every trap, before the kernel's page table is active. Two edits make it use a per-thread trapframe address.

uservec. Delete the lines that save a0 in sscratch and load the constant TRAPFRAME (csrw sscratch, a0 and li a0, TRAPFRAME) and put this in their place:
            # sscratch holds this thread's trapframe address (put there by userret).
            # Swap it with a0: a0 becomes the trapframe address, and sscratch
            # keeps the user's a0.
            csrrw a0, sscratch, a0

Everything after that (saving the registers to 0(a0), 8(a0), and so on, and csrr t0, sscratch / sd t0, 112(a0) to save the user's a0) stays exactly as it is.

userret. It now receives two arguments. Replace the li a0, TRAPFRAME line:
    userret:
            # userret(pagetable, trapframe_address)
            # a0: user page table, for satp
            # a1: this thread's trapframe address

            # switch to the user page table (keep the sfence.vma lines you have)
            csrw satp, a0
            sfence.vma zero, zero

            mv a0, a1                  # was: li a0, TRAPFRAME
            csrw sscratch, a0          # remember it for the next trap

            # restore all registers except a0 from the trapframe (unchanged) ...
            ld ra, 40(a0)
            ...
            # restore user a0 (unchanged)
            ld a0, 112(a0)
            sret

In some older xv6 versions userret is called as userret(TRAPFRAME, satp), with the arguments the other way round. If yours is like that, keep your order: use the register that holds satp for csrw satp, ... and the other one for the trapframe address.

#### Step 2  pass the address from usertrapret

Change the last line of usertrapret():
      uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
      ((void (*)(uint64, uint64))trampoline_userret)(satp, p->tf_va);

p->tf_va is TRAPFRAME for the main thread (set in allocproc) and TFVA(tid) for the others.

Check now: make qemu should still boot and run ls. If it resets or hangs at the first shell prompt, the trampoline edit is wrong. Fix it before going on.

#### Step 3  create  exit and join

Add these prototypes near the top of proc.c, with the other static declarations:
    static void reap_thread(struct proc *pp);

Then add the following functions. reap_thread must appear before exit().
    // Remove a finished thread. The caller holds pp->lock and pp is a ZOMBIE thread.
    static void
    reap_thread(struct proc *pp)
    {
      struct proc *l = pp->leader;

      acquire(&ptlock);
      uvmunmap(l->pagetable, pp->tf_va, 1, 0);     // remove the trapframe mapping
      l->tidmask &= ~(1 << pp->tid);
      l->nthreads--;
      release(&ptlock);

      kfree((void*)pp->trapframe);
      pp->trapframe = 0;
      pp->pagetable = 0;
      pp->leader = 0;
      pp->tid = 0;
      pp->pid = 0;
      pp->name[0] = 0;
      pp->chan = 0;
      pp->killed = 0;
      pp->xstate = 0;
      pp->state = UNUSED;
    }

    // Create a new thread in the calling thread's process.
    //   fn:   function to run        arg:  its argument
    //   sp:   top of the new thread's user stack
    //   stub: user function to return to when fn returns
    // Returns the new thread number (1..MAXTHREADS-1), or -1.
    int
    kthread_create(uint64 fn, uint64 arg, uint64 sp, uint64 stub)
    {
      struct proc *p = myproc();
      struct proc *l = p->leader;
      struct proc *np;
      struct trapframe *tf;
      int tid;

      // 1. reserve a thread number
      acquire(&ptlock);
      for(tid = 1; tid < MAXTHREADS; tid++){
        if((l->tidmask & (1 << tid)) == 0)
          break;
      }
      if(tid == MAXTHREADS){
        release(&ptlock);
        return -1;
      }
      l->tidmask |= (1 << tid);
      l->nthreads++;
      release(&ptlock);

      // 2. a trapframe page, mapped in the shared page table at TFVA(tid)
      if((tf = (struct trapframe *)kalloc()) == 0)
        goto fail;
      acquire(&ptlock);
      int r = mappages(l->pagetable, TFVA(tid), PGSIZE, (uint64)tf, PTE_R | PTE_W);
      release(&ptlock);
      if(r < 0){
        kfree(tf);
        goto fail;
      }

      // 3. a free process-table slot (np->lock is held when the loop breaks)
      for(np = proc; np < &proc[NPROC]; np++){
        acquire(&np->lock);
        if(np->state == UNUSED)
          break;
        release(&np->lock);
      }
      if(np == &proc[NPROC])
        goto fail_map;

      // 4. fill in the new thread
      np->pid = l->pid;                    // threads share the process id
      np->leader = l;
      np->tid = tid;
      np->tf_va = TFVA(tid);
      np->trapframe = tf;
      np->pagetable = l->pagetable;        // the same page table
      np->parent = 0;
      np->killed = 0;
      np->nthreads = 0;
      np->tidmask = 0;
      np->sz = 0;                          // the size lives in the main thread

      memset(tf, 0, sizeof(*tf));
      tf->epc = fn;                        // start running fn(arg) ...
      tf->sp = sp;                         // ... on the new stack
      tf->a0 = arg;
      tf->ra = stub;                       // when fn returns, go to the stub

      // open files and the current directory: this thread gets its own copies
      for(int i = 0; i < NOFILE; i++)
        if(p->ofile[i])
          np->ofile[i] = filedup(p->ofile[i]);
      np->cwd = idup(p->cwd);
      safestrcpy(np->name, p->name, sizeof(p->name));

      memset(&np->context, 0, sizeof(np->context));
      np->context.ra = (uint64)forkret;    // first run: forkret, then back to user mode
      np->context.sp = np->kstack + PGSIZE;

      np->state = RUNNABLE;
      release(&np->lock);
      return tid;

    fail_map:
      acquire(&ptlock);
      uvmunmap(l->pagetable, TFVA(tid), 1, 0);
      release(&ptlock);
      kfree(tf);
    fail:
      acquire(&ptlock);
      l->tidmask &= ~(1 << tid);
      l->nthreads--;
      release(&ptlock);
      return -1;
    }

    // End the calling thread. If it is the main thread the whole process ends.
    void
    kthread_exit(void)
    {
      struct proc *p = myproc();
      struct proc *l = p->leader;

      if(p == l)
        exit(0);

      for(int fd = 0; fd < NOFILE; fd++){
        if(p->ofile[fd]){
          struct file *f = p->ofile[fd];
          fileclose(f);
          p->ofile[fd] = 0;
        }
      }
      begin_op();
      iput(p->cwd);
      end_op();
      p->cwd = 0;

      acquire(&wait_lock);
      wakeup(l);                           // wake a joiner, or the main thread in exit()
      acquire(&p->lock);
      p->state = ZOMBIE;
      release(&wait_lock);
      sched();
      panic("zombie thread exit");
    }

    // Wait for thread tid of this process to finish and free it.
    // Returns 0 on success, -1 if there is no such thread.
    int
    kthread_join(int tid)
    {
      struct proc *p = myproc();
      struct proc *l = p->leader;
      struct proc *pp;

      if(tid <= 0 || tid >= MAXTHREADS || tid == p->tid)
        return -1;

      acquire(&wait_lock);
      for(;;){
        pp = 0;
        for(struct proc *q = proc; q < &proc[NPROC]; q++){
          if(q->leader == l && q->tid == tid && q != l){
            pp = q;
            break;
          }
        }
        if(pp == 0){                        // no such thread, or already joined
          release(&wait_lock);
          return -1;
        }
        acquire(&pp->lock);
        if(pp->state == ZOMBIE){
          reap_thread(pp);
          release(&pp->lock);
          release(&wait_lock);
          return 0;
        }
        release(&pp->lock);
        if(killed(p)){
          release(&wait_lock);
          return -1;
        }
        sleep(l, &wait_lock);               // woken by kthread_exit
      }
    }

The exit sequence in kthread_exit mirrors the real exit(): take wait_lock, wake the sleepers, take the thread's own lock, mark it ZOMBIE, release wait_lock, and switch away. A joiner may free the thread only after the scheduler has switched away from it, and the scheduler releases p->lock only then. That is why the joiner takes pp->lock before it looks at the state.

#### Step 4  make the process level calls thread aware

fork(). The child copies the whole process, but only the calling thread's registers. The child's parent is the process (the main thread), not just the calling thread. Change these lines:
      struct proc *p = myproc();
      struct proc *l = p->leader;               // add
      ...
      if(uvmcopy(l->pagetable, np->pagetable, l->sz) < 0){   // was p->pagetable, p->sz
      ...
      np->sz = l->sz;                            // was p->sz
      ...
      np->parent = l;                            // was p (inside the wait_lock section)

The child is created by allocproc, so it is a normal single-threaded process: leader is itself, tf_va is TRAPFRAME.

wait(). Children belong to the process, so any thread may wait for them. At the top change struct proc *p = myproc(); to:
      struct proc *p = myproc()->leader;        // children belong to the process

and in the sleep condition use the calling thread's own flag: if(!havekids || killed(myproc())). The sleep(p, &wait_lock) call stays; its channel is the main thread, which is exactly what exit() wakes.

kill(). Threads share the pid, so mark every match. Replace kill:
    int
    kill(int pid)
    {
      struct proc *p;
      int found = 0;

      for(p = proc; p < &proc[NPROC]; p++){
        acquire(&p->lock);
        if(p->pid == pid){
          p->killed = 1;
          if(p->state == SLEEPING){
            // Wake the thread from sleep() so it can notice.
            p->state = RUNNABLE;
          }
          found = 1;
        }
        release(&p->lock);
      }
      return found ? 0 : -1;
    }

exit(). Add this block at the very top of exit(int status), after the initproc check:
      if(p != p->leader){
        // A secondary thread called exit(): the whole process must end.
        kill(p->pid);                // marks every thread, the main thread included
        kthread_exit();              // this thread ends; the main thread finishes the exit
      }

      if(p->nthreads > 1){
        // The main thread is leaving: stop and reap all other threads first.
        kill(p->pid);
        acquire(&wait_lock);
        for(;;){
          int alive = 0;
          for(struct proc *q = proc; q < &proc[NPROC]; q++){
            if(q != p && q->leader == p){
              acquire(&q->lock);
              if(q->state == ZOMBIE)
                reap_thread(q);
              else
                alive = 1;
              release(&q->lock);
            }
          }
          if(!alive)
            break;
          sleep(p, &wait_lock);      // woken by kthread_exit
        }
        release(&wait_lock);
      }

The rest of exit (close files, reparent children, become a zombie) stays as it is. By then nthreads is 1 and every thread's trapframe mapping is gone, so freeproc can free the page table safely when the parent calls wait.

exec(). After struct proc *p = myproc(); add:
      if(p->leader != p || p->nthreads > 1)
        return -1;                   // exec in a multi-threaded process is not supported

#### Step 5  the system calls

Same five edits as for memstat in Part 1, for four new calls:

| File | Edit |
| --- | --- |
| kernel/syscall.h | #define SYS_kthread_create 23, SYS_kthread_exit 24, SYS_kthread_join 25, SYS_kgettid 26 (continue your numbering) |
| kernel/syscall.c | extern uint64 sys_kthread_create(void); and so on, plus the four table entries |
| kernel/sysproc.c | the four functions below |
| user/usys.pl | entry("kthread_create"); and the other three |
| user/user.h | the four prototypes below |
    uint64
    sys_kthread_create(void)
    {
      uint64 fn, arg, sp, stub;

      argaddr(0, &fn);
      argaddr(1, &arg);
      argaddr(2, &sp);
      argaddr(3, &stub);
      return kthread_create(fn, arg, sp, stub);
    }

    uint64
    sys_kthread_exit(void)
    {
      kthread_exit();
      return 0;                      // not reached
    }

    uint64
    sys_kthread_join(void)
    {
      int tid;

      argint(0, &tid);
      return kthread_join(tid);
    }

    uint64
    sys_kgettid(void)
    {
      return myproc()->tid;
    }

In user/user.h:
    int kthread_create(uint64, uint64, uint64, uint64);
    void kthread_exit(void);
    int kthread_join(int);
    int kgettid(void);

In kernel/defs.h, in the proc.c section:
    int             kthread_create(uint64, uint64, uint64, uint64);
    void            kthread_exit(void);
    int             kthread_join(int);

#### Step 6  a first test with raw system calls

The library comes in Milestone 8, so this test calls the system calls directly. Each thread needs a stack: we take one from malloc.
    #include "kernel/types.h"
    #include "user/user.h"

    #define STACKSIZE 8192

    static volatile int done[4];            // each thread sets its own slot

    static void
    thread_end(void)                        // the thread functions return here
    {
      kthread_exit();
    }

    static void
    worker(void *arg)
    {
      int id = (int)(uint64)arg;
      done[id] = 1;                         // proves threads share memory
      printf("thread %d running, tid %d\n", id, kgettid());
    }

    int
    main(void)
    {
      int tids[4];

      for(int i = 0; i < 4; i++){
        char *stk = malloc(STACKSIZE);
        uint64 top = ((uint64)stk + STACKSIZE) & ~15UL;
        tids[i] = kthread_create((uint64)worker, (uint64)i, top, (uint64)thread_end);
        if(tids[i] < 0){
          printf("kthreadtest: create failed\n");
          exit(1);
        }
      }
      for(int i = 0; i < 4; i++)
        kthread_join(tids[i]);

      int n = 0;
      for(int i = 0; i < 4; i++)
        n += done[i];
      printf("kthreadtest: %d of 4 threads finished\n", n);
      exit(n == 4 ? 0 : 1);
    }

Add $U/_kthreadtest\ to UPROGS.

#### Step 7  build  run  verify
    make clean
    make qemu


    $ kthreadtest
    thread 0 running, tid 1
    thread 2 running, tid 3
    thread 1 running, tid 2
    thread 3 running, tid 4
    kthreadtest: 4 of 4 threads finished

The order of the first four lines varies; that is normal for parallel threads. Then check:

Run kthreadtest ten times in a row. It must never hang or panic.

Run memstat before and after: free pages must return to its start value, so no thread leaks its trapframe page.

Run usertests (it exercises fork, exit and wait heavily) and confirm it still passes.

Commit the reviewed source and evidence on feature/threads-sync, push it and open the milestone PR. Suggested message: the milestone message in the working agreement. Akshit creates the release tag on validated main after review and merge, using the shared release routine.
    git add kernel user Makefile docs/threads
    git diff --cached --check
    git diff --cached --stat
    git commit -m "v0.6: kernel threads"
    git push origin feature/threads-sync
    # Open the milestone PR then follow the shared review routine

Common mistakes in this milestone

| Symptom | Cause |
| --- | --- |
| reset or hang at the first prompt, before any thread exists | the trampoline.S edit is wrong, or usertrapret still passes one argument |
| scause 0xd or 0xf in a new thread right away | the thread's tf_va was not set, or its trapframe was not mapped in the shared page table |
| panic: freewalk: leaf when a program exits | a thread's trapframe mapping was not removed (check reap_thread and exit) |
| panic: sched locks | kthread_exit released or took locks in a different order than exit() |
| program hangs at exit | the main thread waits in exit() for a thread that never became ZOMBIE; it is probably sleeping and not killable |
| free pages falls after each run | a thread's trapframe page was not freed, or reap_thread was skipped |
| child process of a threaded program misbehaves | fork still uses p->sz or p->pagetable of a secondary thread instead of the main thread's |

### 7  Milestone 7  futex wait and wake

Idea. Two system calls are all a lock needs from the kernel:

futex_wait(addr, expected): if the integer at addr still equals expected, sleep; otherwise return immediately

futex_wake(addr): wake the threads sleeping on addr

The kernel must read the value and go to sleep as one atomic step with respect to futex_wake. We do that by taking a small lock, checking the value under it, and sleeping with sleep(), which releases that lock only after the thread is marked as sleeping. A waker takes the same lock before calling wakeup, so it can never slip in between the check and the sleep.

Continue on feature/threads-sync. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### Step 1  the futex code

A futex is identified by (process, address). We hash that pair into one of 64 buckets. The address of the bucket is the sleep channel, so two unrelated futexes may share a bucket. That only causes a spurious wake-up, and callers always re-check their condition.
    #include "types.h"
    #include "param.h"
    #include "memlayout.h"
    #include "riscv.h"
    #include "spinlock.h"
    #include "proc.h"
    #include "defs.h"

    #define NBUCKET 64

    struct bucket {
      struct spinlock lock;      // makes check-then-sleep atomic against wake
    };
    static struct bucket buckets[NBUCKET];

    static struct bucket *
    bucket_for(struct proc *l, uint64 uaddr)
    {
      return &buckets[(((uint64)l >> 4) ^ (uaddr >> 2)) % NBUCKET];
    }

    void
    futex_init(void)
    {
      for(int i = 0; i < NBUCKET; i++)
        initlock(&buckets[i].lock, "futex");
    }

    // futex_wait(addr, expected): sleep only if *addr still equals expected.
    // Returns 0 when woken or when the value differed, -1 on error or if killed.
    uint64
    sys_futex_wait(void)
    {
      uint64 uaddr;
      int expected, val;
      struct proc *p = myproc();

      argaddr(0, &uaddr);
      argint(1, &expected);

      prefault(uaddr, sizeof(int), 0);     // best effort: make the page resident first
      struct bucket *b = bucket_for(p->leader, uaddr);

      acquire(&b->lock);
      if(copyin(p->pagetable, (char*)&val, uaddr, sizeof(val)) < 0){
        release(&b->lock);
        return -1;
      }
      if(val == expected && !killed(p))
        sleep(b, &b->lock);                // atomically releases the lock while sleeping
      release(&b->lock);
      return killed(p) ? -1 : 0;
    }

    // futex_wake(addr): wake the threads sleeping on addr (all of them).
    uint64
    sys_futex_wake(void)
    {
      uint64 uaddr;
      struct proc *p = myproc();

      argaddr(0, &uaddr);
      struct bucket *b = bucket_for(p->leader, uaddr);
      acquire(&b->lock);
      wakeup(b);
      release(&b->lock);
      return 0;
    }

If your xv6 has no killed() function, use p->killed. Waking all waiters is correct but not minimal: each waiter re-checks its own condition and goes back to sleep if needed.

#### Step 2  wiring

kernel/defs.h: add void futex_init(void);

kernel/main.c: call futex_init(); right after swap_init();

Makefile: add $K/futex.o \ to OBJS

kernel/syscall.h: #define SYS_futex_wait 27 and #define SYS_futex_wake 28

kernel/syscall.c: extern uint64 sys_futex_wait(void);, extern uint64 sys_futex_wake(void); and the two table entries

user/usys.pl: entry("futex_wait"); and entry("futex_wake");

user/user.h: int futex_wait(int*, int); and int futex_wake(int*);

#### Step 3  test
    #include "kernel/types.h"
    #include "user/user.h"

    static volatile int flag;

    static void
    thread_end(void)
    {
      kthread_exit();
    }

    static void
    waiter(void *arg)
    {
      while(flag == 0)
        futex_wait((int*)&flag, 0);        // sleeps only while flag is still 0
      printf("futextest: waiter woke up, flag=%d\n", flag);
    }

    int
    main(void)
    {
      char *stk = malloc(8192);
      uint64 top = ((uint64)stk + 8192) & ~15UL;
      int tid = kthread_create((uint64)waiter, 0, top, (uint64)thread_end);

      sleep(10);                           // give the waiter time to go to sleep
      flag = 1;
      __sync_synchronize();                // make the store visible before waking
      futex_wake((int*)&flag);

      kthread_join(tid);
      printf("futextest: OK\n");
      exit(0);
    }

Add $U/_futextest\ to UPROGS, then make clean; make qemu. Expected:
    $ futextest
    futextest: waiter woke up, flag=1
    futextest: OK

Optional: change sleep(10) to sleep(300) and press Ctrl-p while it waits. The process list shows the waiter in state sleep, which proves it is not burning CPU.

Commit the reviewed source and evidence on feature/threads-sync, push it and open the milestone PR. Suggested message: v0.7: futex. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

Common mistakes

| Symptom | Cause |
| --- | --- |
| waiter sleeps forever | the value was changed after futex_wake, or futex_wake was never called; always change the value first, then wake |
| waiter never sleeps, program spins | expected does not match the real value, so futex_wait returns at once |
| panic: sched locks | something other than the bucket lock was held when calling sleep |
| panic: acquire | futex_wait called prefault or copyin that tried to take a lock this code already held; keep prefault before taking the bucket lock |

### 8  Milestone 8  the thread library

Idea. User programs should not hand-build stacks and call raw system calls. The library hides that behind thread_create/thread_join and builds the mutex, condition variable and semaphore on futex_wait and futex_wake.

Continue on feature/threads-sync. Commit the previous milestone first, merge the latest origin/main, then implement this milestone. Do not create the original guide branch name.

#### Step 1  the header
    #ifndef USER_THREAD_H
    #define USER_THREAD_H

    typedef struct { volatile int state; } mutex_t;   // 0 free, 1 locked, 2 locked and someone waits
    typedef struct { volatile int seq; } cond_t;      // counts signals
    typedef struct {
      mutex_t m;
      cond_t c;
      int count;
    } sem_t;

    int  thread_create(void (*fn)(void *), void *arg); // returns thread number, or -1
    int  thread_join(int tid);
    void thread_exit(void);
    int  thread_self(void);

    void mutex_init(mutex_t *m);
    void mutex_lock(mutex_t *m);
    void mutex_unlock(mutex_t *m);

    void cond_init(cond_t *c);
    void cond_wait(cond_t *c, mutex_t *m);
    void cond_signal(cond_t *c);
    void cond_broadcast(cond_t *c);

    void sem_init(sem_t *s, int value);
    void sem_wait(sem_t *s);
    void sem_post(sem_t *s);

    #endif

#### Step 2  the implementation
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define STACKSIZE 16384
    #define MAXT 8                          // same as MAXTHREADS in the kernel

    static char *stacks[MAXT];              // user stack of each thread, indexed by thread number

    // A thread function "returns" here, because the kernel put this address in
    // the new thread's return-address register.
    static void
    thread_start_stub(void)
    {
      kthread_exit();
    }

    int
    thread_create(void (*fn)(void *), void *arg)
    {
      char *stk = malloc(STACKSIZE);
      if(stk == 0)
        return -1;
      uint64 top = ((uint64)stk + STACKSIZE) & ~15UL;      // stacks are 16-byte aligned
      int tid = kthread_create((uint64)fn, (uint64)arg, top, (uint64)thread_start_stub);
      if(tid < 0){
        free(stk);
        return -1;
      }
      stacks[tid] = stk;
      return tid;
    }

    int
    thread_join(int tid)
    {
      if(kthread_join(tid) < 0)
        return -1;
      free(stacks[tid]);
      stacks[tid] = 0;
      return 0;
    }

    void
    thread_exit(void)
    {
      kthread_exit();
    }

    int
    thread_self(void)
    {
      return kgettid();
    }

    // ---- mutex: three states, the kernel is entered only under contention --------

    void
    mutex_init(mutex_t *m)
    {
      m->state = 0;
    }

    void
    mutex_lock(mutex_t *m)
    {
      // fast path: free (0) -> locked (1) with one atomic instruction
      if(__sync_val_compare_and_swap(&m->state, 0, 1) == 0)
        return;
      // slow path: mark "locked, with waiters" (2) and sleep until we get it
      while(__sync_lock_test_and_set(&m->state, 2) != 0)
        futex_wait((int*)&m->state, 2);
    }

    void
    mutex_unlock(mutex_t *m)
    {
      __sync_synchronize();                 // publish the critical section's writes first
      if(__sync_lock_test_and_set(&m->state, 0) == 2)
        futex_wake((int*)&m->state);        // somebody may be asleep
    }

    // ---- condition variable: a counter of signals --------------------------------

    void
    cond_init(cond_t *c)
    {
      c->seq = 0;
    }

    void
    cond_wait(cond_t *c, mutex_t *m)
    {
      int seq = c->seq;                     // read while we still hold the mutex
      mutex_unlock(m);
      futex_wait((int*)&c->seq, seq);       // sleeps only if nobody signalled since the read
      mutex_lock(m);
    }

    void
    cond_signal(cond_t *c)
    {
      __sync_fetch_and_add(&c->seq, 1);     // change the value first ...
      futex_wake((int*)&c->seq);            // ... then wake
    }

    void
    cond_broadcast(cond_t *c)
    {
      cond_signal(c);                       // futex_wake wakes every waiter anyway
    }

    // ---- counting semaphore -------------------------------------------------------

    void
    sem_init(sem_t *s, int value)
    {
      mutex_init(&s->m);
      cond_init(&s->c);
      s->count = value;
    }

    void
    sem_wait(sem_t *s)
    {
      mutex_lock(&s->m);
      while(s->count == 0)
        cond_wait(&s->c, &s->m);
      s->count--;
      mutex_unlock(&s->m);
    }

    void
    sem_post(sem_t *s)
    {
      mutex_lock(&s->m);
      s->count++;
      cond_signal(&s->c);
      mutex_unlock(&s->m);
    }

Why three mutex states? With only 0 and 1, every unlock would have to call the kernel in case somebody sleeps. State 2 records that a waiter exists, so an uncontended lock and unlock never enter the kernel.

Why does cond_wait read seq before unlocking? If a signal arrives between the unlock and the sleep, seq has changed, so futex_wait sees a different value and returns at once. That is how a lost wakeup is avoided.

Why while, not if, around every cond_wait? Wake-ups may be spurious (our futex_wake wakes everybody), so the condition must always be re-tested.

#### Step 3  link it into every program

Find the ULIB line and append the new object:
    ULIB = $U/ulib.o $U/usys.o $U/printf.o $U/umalloc.o $U/thread.o

#### Step 4  test the library

A parallel sum: each thread adds its own part of an array, so no lock is needed.
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define N  1000
    #define NT 4

    static int data[N];
    static int partial[NT];

    static void
    sum_part(void *arg)
    {
      int id = (int)(uint64)arg;
      int s = 0;
      for(int i = id * (N / NT); i < (id + 1) * (N / NT); i++)
        s += data[i];
      partial[id] = s;
    }

    int
    main(void)
    {
      int tids[NT];

      for(int i = 0; i < N; i++)
        data[i] = i;
      for(int i = 0; i < NT; i++)
        tids[i] = thread_create(sum_part, (void*)(uint64)i);
      for(int i = 0; i < NT; i++)
        thread_join(tids[i]);

      int total = 0;
      for(int i = 0; i < NT; i++)
        total += partial[i];
      printf("threadtest: sum = %d (expected %d)\n", total, N * (N - 1) / 2);
      exit(total == N * (N - 1) / 2 ? 0 : 1);
    }

Add $U/_threadtest\ to UPROGS. Expected: threadtest: sum = 499500 (expected 499500).

Known limitation to remember in the report: xv6's malloc and free are not thread-safe. In these programs only the main thread calls them (thread_create and thread_join). If your own thread code needs malloc, protect it with a mutex.

Commit the reviewed source and evidence on feature/threads-sync, push it and open the milestone PR. Suggested message: v0.8: thread library. Akshit creates the release tag on validated main after review and merge, using the shared release routine.

### 9  Milestone 9  demos and tests

These programs are your evidence. Run them with the default make qemu (3 CPUs) so the threads truly run in parallel.

#### 9 1 The race  and the fix
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define NTHREADS 4
    #define ITERS    50000

    static volatile int counter;            // shared, unprotected
    static volatile int counter_locked;     // shared, protected by the mutex
    static mutex_t lock;

    static void
    unsafe_worker(void *arg)
    {
      for(int i = 0; i < ITERS; i++)
        counter++;                          // read, add, write: not atomic
    }

    static void
    safe_worker(void *arg)
    {
      for(int i = 0; i < ITERS; i++){
        mutex_lock(&lock);
        counter_locked++;
        mutex_unlock(&lock);
      }
    }

    int
    main(void)
    {
      int tids[NTHREADS];

      mutex_init(&lock);

      for(int i = 0; i < NTHREADS; i++)
        tids[i] = thread_create(unsafe_worker, 0);
      for(int i = 0; i < NTHREADS; i++)
        thread_join(tids[i]);
      printf("without lock: expected %d, got %d\n", NTHREADS * ITERS, counter);

      for(int i = 0; i < NTHREADS; i++)
        tids[i] = thread_create(safe_worker, 0);
      for(int i = 0; i < NTHREADS; i++)
        thread_join(tids[i]);
      printf("with mutex:   expected %d, got %d\n", NTHREADS * ITERS, counter_locked);
      exit(0);
    }

Expected (the first number varies and is lower than 200000):
    without lock: expected 200000, got 143512
    with mutex:   expected 200000, got 200000

If the first number is exactly 200000, the threads happened not to collide. Run it again, or with ITERS larger.

#### 9 2 Producer and consumer

A bounded buffer with a mutex and two condition variables.
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define BUFSZ 8
    #define ITEMS 100

    static int buf[BUFSZ];
    static int head, tail, count;
    static mutex_t m;
    static cond_t not_full, not_empty;
    static int total;
    static mutex_t total_lock;

    static void
    producer(void *arg)
    {
      for(int i = 1; i <= ITEMS; i++){
        mutex_lock(&m);
        while(count == BUFSZ)
          cond_wait(&not_full, &m);
        buf[tail] = i;
        tail = (tail + 1) % BUFSZ;
        count++;
        cond_signal(&not_empty);
        mutex_unlock(&m);
      }
    }

    static void
    consumer(void *arg)
    {
      for(int n = 0; n < ITEMS; n++){
        mutex_lock(&m);
        while(count == 0)
          cond_wait(&not_empty, &m);
        int v = buf[head];
        head = (head + 1) % BUFSZ;
        count--;
        cond_signal(&not_full);
        mutex_unlock(&m);

        mutex_lock(&total_lock);
        total += v;
        mutex_unlock(&total_lock);
      }
    }

    int
    main(void)
    {
      int tids[4];

      mutex_init(&m);
      mutex_init(&total_lock);
      cond_init(&not_full);
      cond_init(&not_empty);

      tids[0] = thread_create(producer, 0);
      tids[1] = thread_create(producer, 0);
      tids[2] = thread_create(consumer, 0);
      tids[3] = thread_create(consumer, 0);
      for(int i = 0; i < 4; i++)
        thread_join(tids[i]);

      printf("prodcons: total %d (expected %d)\n", total, 2 * ITEMS * (ITEMS + 1) / 2);
      exit(0);
    }

Expected: prodcons: total 10100 (expected 10100).

#### 9 3 Dining philosophers

Five philosophers share five forks (mutexes). Every philosopher takes the lower-numbered fork first. That fixed lock order makes a cycle of waiting impossible.
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define NPHIL 5
    #define MEALS 20

    static mutex_t forks[NPHIL];
    static int meals[NPHIL];

    static void
    philosopher(void *arg)
    {
      int id = (int)(uint64)arg;
      int left = id;
      int right = (id + 1) % NPHIL;
      int first = left < right ? left : right;     // lock order: lower number first
      int second = left < right ? right : left;

      for(int i = 0; i < MEALS; i++){
        mutex_lock(&forks[first]);
        mutex_lock(&forks[second]);
        meals[id]++;                               // eating
        mutex_unlock(&forks[second]);
        mutex_unlock(&forks[first]);
      }
    }

    int
    main(void)
    {
      int tids[NPHIL];

      for(int i = 0; i < NPHIL; i++)
        mutex_init(&forks[i]);
      for(int i = 0; i < NPHIL; i++)
        tids[i] = thread_create(philosopher, (void*)(uint64)i);
      for(int i = 0; i < NPHIL; i++)
        thread_join(tids[i]);

      for(int i = 0; i < NPHIL; i++)
        printf("philosopher %d ate %d times\n", i, meals[i]);
      exit(0);
    }

Expected: every philosopher ate 20 times and the program ends.

Optional experiment (explain it in the report). Change the code so each philosopher takes the left fork first and then the right fork, and add sleep(1) between the two mutex_lock calls. All five now hold a left fork and wait for a right fork, so the program hangs: a deadlock. Quit QEMU with Ctrl-a then x, and press Ctrl-p before that to show the threads sleeping.

#### 9 4 Exit from a thread
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    static void
    worker(void *arg)
    {
      sleep(5);
      printf("exittest: thread calls exit(3)\n");
      exit(3);                               // must end the WHOLE process
    }

    int
    main(void)
    {
      int tid = thread_create(worker, 0);
      thread_join(tid);                      // never returns normally: the process is killed
      printf("exittest: FAIL, main thread survived\n");
      exit(1);
    }

Expected: the line thread calls exit(3) appears, then the shell prompt returns, and FAIL is never printed. Afterwards run memstat and check that free pages is back to its earlier value.

#### 9 5 Parallel speedup

This shows that threads really run on several CPUs.
    #include "kernel/types.h"
    #include "user/user.h"
    #include "user/thread.h"

    #define TOTAL 40000000

    static void
    spin(void *arg)
    {
      int n = (int)(uint64)arg;
      volatile int x = 0;
      for(int i = 0; i < n; i++)
        x += i;
    }

    int
    main(int argc, char *argv[])
    {
      int nt = argc > 1 ? atoi(argv[1]) : 1;
      int tids[8];

      if(nt < 1 || nt > 7)
        nt = 1;
      int start = uptime();
      for(int i = 0; i < nt; i++)
        tids[i] = thread_create(spin, (void*)(uint64)(TOTAL / nt));
      for(int i = 0; i < nt; i++)
        thread_join(tids[i]);
      printf("speedup: %d thread(s) took %d ticks\n", nt, uptime() - start);
      exit(0);
    }

Run speedup 1, speedup 2 and speedup 3 on the same 3 CPU 128 MB build and record several tick observations per case. Report what the emulator actually measures; a proportional speedup is not guaranteed by correctness.

Add all five programs to UPROGS (racetest, prodcons, philosophers, exittest, speedup). Commit and push the reviewed source and evidence on feature/threads-sync, then open the milestone PR. Create the release tag only on validated main after merge.

### 10  Final test and demo checklist

| Configuration | Command | Expected |
| --- | --- | --- |
| default (3 CPUs, 128 MB) | usertests | ALL TESTS PASSED |
| default | kthreadtest, futextest, threadtest | all report success |
| default | racetest | unprotected count too low, mutex count exact |
| default | prodcons | total 10100 |
| default | philosophers | every philosopher ate 20 times |
| default | exittest | prompt returns, no FAIL |
| default | speedup 1, 2, 3 | Record repeated ticks and report the measured speedup under the same configuration. |
| every run | memstat before and after | free pages returns to the start value (no leaked trapframes or stacks) |
| CPUS=1 RAM_MB=8 | Part 1 swap tests (memhog 12, swapfork) | still pass |

Run every program at least ten times. Concurrency bugs are rare, so one clean run proves little.

Demo script (about 4 minutes): (1) racetest: the lost updates and the fix. (2) prodcons. (3) speedup 1 against speedup 3. (4) exittest and memstat showing no leak. (5) Draw the design diagram: one page table, several proc entries, one trapframe address each.

### 11  Report outline and viva questions

Report outline: (1) motivation; (2) background: threads, races, locks, futexes; (3) design: thread as a proc, shared address space, per-thread trapframe; (4) implementation: files changed (git diff --stat v0.4..v0.9); (5) results: race demo, speedup table, leak check; (6) limitations and future work; (7) conclusion.

| Question | Short answer |
| --- | --- |
| Why does each thread need its own kernel stack? | Two threads can be inside system calls or asleep at the same time, and each needs its own call stack |
| Why a trapframe per thread? | A trap saves the registers of the thread that trapped. Two threads in one page table cannot share one trapframe address |
| Why does the scheduler not change? | A thread is just another runnable entry in the process table |
| What is a lost wakeup and how do you avoid it? | A wake that happens between the value check and the sleep. We check and sleep under one lock that the waker also takes |
| Why does the mutex have three states? | State 2 means "waiters exist", so an uncontended unlock never calls the kernel |
| Why while around cond_wait? | Wake-ups can be spurious, so the condition must be re-tested |
| What happens when a secondary thread calls exit()? | All threads are marked killed; the main thread waits for the others, frees them, then exits |
| Why is exec refused in a multi-threaded process? | It would have to replace the address space under the other threads |
| Why ptlock around page-table changes? | Two CPUs could otherwise map the same page twice or update the same entry at once |
| Deadlock versus race? | A race corrupts data through unsynchronized access. A deadlock is threads waiting for each other forever. Lock ordering prevents the second |
| Why is xv6's malloc not thread-safe? | It keeps a global free list with no lock |

### 12  Troubleshooting reference

| Symptom | Likely cause | What to check |
| --- | --- | --- |
| reset or hang at the first prompt (before any thread exists) | trampoline or usertrapret edit wrong | Milestone 6, steps 1 and 2 |
| usertrap(): unexpected scause 0xd right after kthread_create | the thread's tf_va not set, or its trapframe not mapped | kthread_create step 2 and 4 |
| usertrap(): unexpected scause 0x2 (illegal instruction) in a thread | bad function pointer, or stack not aligned to 16 bytes | the top value and the stub address |
| panic: freewalk: leaf | a thread trapframe mapping was not removed | reap_thread, and the loop in exit() |
| panic: remap | two threads mapped the same page | the handler must hold ptlock and re-check after every sleep |
| panic: acquire | the same CPU took ptlock twice | callers of pin_user_page must release before calling again |
| panic: sched locks | slept while holding a spinlock | futex_wait, kthread_exit lock order |
| program hangs at exit | the main thread waits for a thread that is not killable | threads sleeping in code that never checks killed |
| waiters sleep forever | wake before change, or missing barrier | change the value, __sync_synchronize(), then futex_wake |
| mutex count wrong | volatile missing, or a barrier missing in mutex_unlock | thread.c |
| strange crash after fork in a threaded program | fork copied from a secondary thread | use the main thread's size and page table |
| free pages falls with every run | trapframe page or stack not freed | reap_thread, thread_join frees stacks[tid] |

### 13  Known limitations

File descriptors are copied when a thread is created. A file opened later by one thread is not visible to the others.

malloc is not thread-safe. Only the main thread should call it, or protect it with a mutex.

futex_wake wakes every waiter on that bucket. This is correct but not as efficient as waking one.

fork() in a multi-threaded program on several CPUs is not safe unless the other threads are idle: we flush only the local TLB when parent pages become copy-on-write, and xv6 has no TLB shootdown between CPUs.

Swap needs one CPU. Combine swap and threads only with CPUS=1.

exec() is refused in a process with more than one thread. At most 8 threads per process.

Threads share the pid, so kill(pid) kills the whole process, and no signal or debugger support exists for individual threads (Part 3).

### 14  What comes next

Part 3 adds signals, the trace system call and the breakpoint debugger. Tag v0.9 as the stable base. A signal sent to a multi-threaded process will be delivered to its main thread, and the debugger will handle single-threaded programs only; both are documented as limitations.
