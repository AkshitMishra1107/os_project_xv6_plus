# Primary references and their use

The report maps the following established mechanisms to the implementation and its experiments. MIT-lab algorithms are attributed; project engineering includes shared-state integration, diagnostics, lifecycle repairs and checked workloads.

1. Microsoft, “What is the Windows Subsystem for Linux?” [Primary source](https://learn.microsoft.com/en-us/windows/wsl/about). Report use: Sections 2.1 and 4.1.

2. R. Cox, F. Kaashoek, and R. Morris, xv6: a simple, Unix-like teaching operating system, rev. 5, MIT, 2025. [Primary source](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf). Report use: Sections 2.1 and 4.2.

3. RISC-V International, The RISC-V Instruction Set Manual, Volume II: Privileged Architecture, Supervisor-Level ISA. [Primary source](https://docs.riscv.org/reference/isa/v20260120/priv/supervisor.html). Report use: Sections 2.1, 4.3 and 4.4.

4. MIT PDOS, “Lab: xv6 lazy page allocation,” 6.S081, 2020. [Primary source](https://pdos.csail.mit.edu/6.S081/2020/labs/lazy.html). Report use: Sections 2.2 and 4.3.

5. MIT PDOS, “Lab: Copy-on-Write Fork for xv6,” 6.1810, 2025. [Primary source](https://pdos.csail.mit.edu/6.1810/2025/labs/cow.html). Report use: Sections 2.2, 4.3 and 5.4.

6. R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces, ch. 21, “Beyond Physical Memory: Mechanisms.” [Primary source](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf). Report use: Section 4.4.

7. R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces, ch. 22, “Beyond Physical Memory: Policies.” [Primary source](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys-policy.pdf). Report use: Sections 4.4 and 5.3.

8. P. J. Denning, “The working set model for program behavior,” Communications of the ACM, vol. 11, no. 5, pp. 323–333, May 1968, doi: 10.1145/363095.363141. [Primary source](https://doi.org/10.1145/363095.363141). Report use: Sections 2.2 and 5.3.

9. L. A. Belady, R. A. Nelson, and G. S. Shedler, “An anomaly in space-time characteristics of certain programs running in a paging machine,” Communications of the ACM, vol. 12, no. 6, pp. 349–353, Jun. 1969, doi: 10.1145/363011.363155. [Primary source](https://research.ibm.com/publications/an-anomaly-in-space-time-characteristics-of-certain-programs-running-in-a-paging-machine). Report use: Sections 2.2 and 5.3.

10. R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces, ch. 28, “Locks.” [Primary source](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf). Report use: Section 4.5.

11. R. H. Arpaci-Dusseau and A. C. Arpaci-Dusseau, Operating Systems: Three Easy Pieces, ch. 30, “Condition Variables.” [Primary source](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf). Report use: Section 4.5.

12. MIT PDOS, “Lab: System calls,” 6.S081, 2020. [Primary source](https://pdos.csail.mit.edu/6.S081/2020/labs/syscall.html). Report use: Sections 4.4 and 4.6.

13. MIT PDOS, “Lab: Traps,” 6.S081, 2020. [Primary source](https://pdos.csail.mit.edu/6.S081/2020/labs/traps.html). Report use: Section 4.6.

14. MIT PDOS, “RISC-V Calling Convention,” lecture slides, 2024. [Primary source](https://pdos.csail.mit.edu/6.828/2024/lec/l-riscv-cc-slides.pdf). Report use: Section 4.6.

15. Free Software Foundation, “RISC-V Options,” GCC online documentation. [Primary source](https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html). Report use: Section 4.1.

16. MIT PDOS, “Lab: Multithreading,” 6.1810, 2023. [Primary source](https://pdos.csail.mit.edu/6.1810/2023/labs/thread.html). Report use: Sections 2.2 and 4.5.

17. F. J. Corbató, “A Paging Experiment with the Multics System,” Project MAC memorandum MAC-M-384, Jul. 8, 1968. [Primary source](https://people.csail.mit.edu/saltzer/Multics/MHP-Saltzer-060508/bookcases/M00s/M0104%20074-12%29.PDF). Report use: Section 4.4.

18. H. Franke, R. Russell, and M. Kirkwood, “Fuss, Futexes and Furwocks: Fast Userlevel Locking in Linux,” Ottawa Linux Symposium, 2002. [Primary source](https://www.kernel.org/doc/ols/2002/ols2002-pages-479-495.pdf). Report use: Section 4.5.

19. IEEE and The Open Group, “sigaction,” POSIX Programmer’s Manual, authorized specification reprint. [Primary source](https://man7.org/linux/man-pages/man3/sigaction.3p.html). Report use: Section 4.6.

20. The Linux Kernel Documentation, “Memory Management Concepts.” [Primary source](https://cdn.kernel.org/doc/html/latest/admin-guide/mm/concepts.html). Report use: Section 2.2.

21. M. Kerrisk, “fork(2),” Linux man-pages. [Primary source](https://man7.org/linux/man-pages/man2/fork.2.html). Report use: Section 2.2.

22. D. S. Miller, “Cache and TLB Flushing Under Linux,” The Linux Kernel Documentation. [Primary source](https://docs.kernel.org/core-api/cachetlb.html). Report use: Section 6.
