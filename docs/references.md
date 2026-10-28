# References and attribution

These primary references were checked on 28 October 2026 while preparing the memory report draft. They document the techniques and upstream software; they do not certify this repository or imply an MIT grader pass. Newer xv6 versions differ from our pinned eager baseline.

## How the project relates to the labs

| Project work | Reference | Relationship |
|---|---|---|
| Lazy allocation | R3 | Direct conceptual match to the archived 2020 lab; our handler also integrates COW and swap. |
| COW fork | R4 | Direct conceptual match; our cowtest is a project-specific test, not MIT's complete grader. |
| memstat | R5 | Related to sysinfo; our API has seven memory fields rather than the same sysinfo structure. |
| Swap and replacement | R6 and R7 | Standard VM mechanisms and policy references; no identical MIT swap assignment is asserted. |
| xv6 foundation | R1, R2 and R12 | Official operating-system source and book; inherited copyright and history are retained. |

## Verified primary sources

### R1 MIT PDOS xv6 for RISC V source repository

[MIT PDOS xv6 for RISC V source repository](https://github.com/mit-pdos/xv6-riscv)

The upstream operating system and retained MIT license. The project's eager baseline is pinned to c4de4b0 and checkpoint v0.0; newer upstream versions differ.

### R2 xv6 a simple Unix like teaching operating system

[xv6 a simple Unix like teaching operating system](https://mit-pdos.github.io/xv6-riscv-book/)

Official book by Russ Cox, Frans Kaashoek and Robert Morris. Background reading for page tables, traps, allocation, locking and the xv6 source.

### R3 MIT 6 S081 2020 lazy page allocation lab

[MIT 6 S081 2020 lazy page allocation lab](https://pdos.csail.mit.edu/6.S081/2020/labs/lazy.html)

Direct assignment correspondence for deferred sbrk allocation and fault-driven zero-filled pages. This report uses the archived year explicitly.

### R4 MIT 6 1810 2025 copy on write fork lab

[MIT 6 1810 2025 copy on write fork lab](https://pdos.csail.mit.edu/6.1810/2025/labs/cow.html)

Direct assignment correspondence for read-only sharing, COW recovery, reference counts and copyout integration. Our custom cowtest is not MIT's full lab grader.

### R5 MIT 6 S081 2020 system calls lab

[MIT 6 S081 2020 system calls lab](https://pdos.csail.mit.edu/6.S081/2020/labs/syscall.html)

The sysinfo exercise is related to kernel information collection. Our seven-field memstat API is a project-specific extension, not the identical sysinfo interface.

### R6 OSTEP chapter 21 Beyond Physical Memory Mechanisms

[OSTEP chapter 21 Beyond Physical Memory Mechanisms](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf)

Primary textbook reference by Remzi H Arpaci-Dusseau and Andrea C Arpaci-Dusseau for disk-backed memory and page-in/page-out mechanisms.

### R7 OSTEP chapter 22 Beyond Physical Memory Policies

[OSTEP chapter 22 Beyond Physical Memory Policies](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys-policy.pdf)

Primary textbook reference for replacement-policy reasoning including FIFO and approximations based on recent use. The project results do not establish universal policy superiority.

### R8 RISC V supervisor level ISA version 1 13

[RISC V supervisor level ISA version 1 13](https://docs.riscv.org/reference/isa/v20260120/priv/supervisor.html)

Official specification for page-table permission and software bits, accessed bits, page-fault causes and SFENCE.VMA. The reference is pinned to the published 20260120 documentation version.

### R9 GCC RISC V target options

[GCC RISC V target options](https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html)

Official documentation for the march target selection used in the recorded build-compatibility repair.

### R10 Microsoft Learn Windows Subsystem for Linux overview

[Microsoft Learn Windows Subsystem for Linux overview](https://learn.microsoft.com/en-us/windows/wsl/about)

Primary environment reference for Ubuntu on Windows and WSL2's managed Linux-kernel virtual machine; xv6 runs separately inside QEMU.

### R11 Current xv6 book page faults chapter

[Current xv6 book page faults chapter](https://mit-pdos.github.io/xv6-riscv-book/pgfault.html)

Explains the current upstream lazy allocation path. This is why our historical eager checkpoint, rather than a claim about all current xv6 releases, is the comparison reference.

### R12 MIT PDOS xv6 RISC V book source repository

[MIT PDOS xv6 RISC V book source repository](https://github.com/mit-pdos/xv6-riscv-book)

Official source repository for the xv6 book; included as a second authentic MIT repository reference rather than an unverified student solution.

### R13 MIT 6 S081 2020 utilities lab and starter repository provenance

[MIT 6 S081 2020 utilities lab and starter repository provenance](https://pdos.csail.mit.edu/6.S081/2020/labs/util.html)

Documents the course-specific xv6-labs-2020 starter repository and explains its difference from the book's xv6-riscv source. Its historical clone command is not a project build instruction.

## What this repository claims

This is an educational implementation and integration on MIT xv6, using the supplied project guides with assisted development and debugging. The contribution is the project code, combined subsystem behavior, test design, recorded repairs and evidence. The techniques are established. References were added during the draft review; no audit establishing code lineage from unrelated student solution repositories is claimed.

Use official repositories and course pages for provenance. Preserve the original LICENSE. Keep historical v0.0/v0.4 comparisons distinct from current main and do not label custom tests as official MIT grading results.
