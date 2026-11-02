Current validation, native captures and controlled benchmarks: [validation revision](../validation-upgrade/README.md). This page retains its historical source and results.

# Final integrated validation

Real QEMU executions captured on 2 November 2026. The combined code preserves the original teammate commits and integrates them with the real thread library. Kernel/user/build code is frozen at integration commit `09d752e`; later presentation changes do not change this source.

The later [matched comparison study](../comparison-study/README.md) adds repeated eager/extended fork measurements, three fresh guest boots per paging policy, input/output captures and another complete three-CPU `usertests` pass. Its measurements remain separate from the ordered-session captures below.

| Run | Configuration | Result |
|---|---|---|
| Earlier full SMP check | 3 CPUs, 128 MiB, policy 1 | Full usertests passed before the final debugger orphan repair |
| Final SMP check | 3 CPUs, 128 MiB, policy 1 | Focused commands and complete usertests passed including the lifecycle repair |
| Final single-CPU check | 1 CPU, 128 MiB, policy 1 | Focused tests passed; full-suite capture timed out at outofinodes after 20 minutes and is incomplete |
| Final FIFO pressure | 1 CPU, 8 MiB, policy 1 | 4/12 MiB memhog, swapfork, shared threadswap and hotcold completed |
| Final Clock pressure | 1 CPU, 8 MiB, policy 2 | Same ordered workload completed |

## Integration repairs

- Replaced the development branch's sequential placeholder header with the actual kernel-thread and futex-backed user library. finaldemo checks create/join and prints real nonzero TIDs.
- Preserved access-specific faults, per-member trapframes and all syscall numbers. Added trace inheritance for new members and names for thread/futex calls.
- Updated debugger code writes to release ptlock according to pin_user_page's integrated contract.
- Limited debugger targets to an opted-in single-context direct child and reject thread creation by a traced target.
- Fixed debugger abandonment: reparent already holds wait_lock, so it calls the internal group-stop helper rather than kill, which would acquire the same lock again. dbglife exercises eight abandoned targets.
- Stop best-effort frame unwinding at the entry stub rather than interpreting older stack data as a caller.
- Route process-directed signals to the group leader. This remains a compact Unix-inspired API, not POSIX signals.

## Selected actual output

### smp-final

```text
forkbench
forkbench: 50 forks of a 20 MB process took 6 ticks
$
```

```text
dbglife
dbglife: OK (8 abandoned targets and traced thread guard)
$
```

```text
finaldemo
finaldemo: worker 0 running as tid 1
finaldemo: worker 1 running as tid 2
finaldemo: worker 2 running as tid 3
finaldemo: 3 threads touched 1536 pages, signal 10 received
finaldemo: lazy allocations 1541, page faults 1544
$
```

[Selected console record](outputs/smp-final.txt)

### onecpu-final

```text
forkbench
forkbench: 50 forks of a 20 MB process took 6 ticks
$
```

```text
dbglife
dbglife: OK (8 abandoned targets and traced thread guard)
$
```

```text
finaldemo
finaldemo: worker 0 running as tid 1
finaldemo: worker 1 running as tid 2
finaldemo: worker 2 running as tid 3
finaldemo: 3 threads touched 1536 pages, signal 10 received
finaldemo: lazy allocations 1541, page faults 1544
$
```

[Selected console record](outputs/onecpu-final.txt)

### fifo-final

```text
memhog 12
memhog: starting 12 MB workload
memhog: 12 MB, 3072 pages, 0 bad, swap outs 4386, swap ins 3072
memhog: child exit status 0
$
```

```text
swapfork
swapfork: child bad=0
swapfork: OK
$
```

```text
threadswap
threadswap: OK (4 workers, 12 MB, 3072 pages, 0 bad)
threadswap: swap outs 2870, swap ins 1548 (workload deltas)
$
```

```text
hotcold
hotcold: 12 MB, 3 rounds
hotcold: swap ins 8006, swap outs 8007
$
```

[Selected console record](outputs/fifo-final.txt)

### clock

```text
memhog 12
memhog: starting 12 MB workload
memhog: 12 MB, 3072 pages, 0 bad, swap outs 4386, swap ins 3068
memhog: child exit status 0
$
```

```text
swapfork
swapfork: child bad=0
swapfork: OK
$
```

```text
threadswap
threadswap: OK (4 workers, 12 MB, 3072 pages, 0 bad)
threadswap: swap outs 4387, swap ins 3076 (workload deltas)
$
```

```text
hotcold
hotcold: 12 MB, 3 rounds
hotcold: swap ins 7611, swap outs 7616
$
```

[Selected console record](outputs/clock.txt)

## Evidence and limits

[Final SMP full usertests](outputs/smp-final-usertests.txt) · [Incomplete one-CPU full-suite capture](outputs/onecpu-final-incomplete-usertests.txt) · [Earlier SMP full usertests](outputs/smp-reviewed-usertests.txt) · [Source/configuration/hash index](validation.json).

The policy runs use the same frozen source and ordered command sequence. Their hotcold values are measured-phase transfer deltas, not execution-time measurements or a universal winner. Historical eager/COW and pre-pipe-repair policy observations remain separately labelled in the memory result page and report.

Swapping is one-CPU only. Remaining occupied slots were not audited to owners, so stable counts are not called leak freedom. The original usertests source was not weakened. The first capture's trace-event assertion was corrected to allow the leader's expected identity lookup; the later runs include the corrected test. No generated terminal screenshot or expected-output substitution is used here.
