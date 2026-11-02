# Report captures and provenance

This supplement supports the final report figures. It combines the supplied historical memory screenshots with additional commands executed in WSL on 3 November 2026. The [evidence map](figure-evidence-map.json) records each source hash, dimensions, display crop and evidence type. The [capture archive](evidence.zip) preserves full images, serial logs, command records, build logs and source/kernel hashes.

## Evidence groups

- **Original development captures:** lazy reservation, COW failure/success and tick timing, memory counters, pressure before/after paging, swapfork and FIFO/Clock hotcold. The exact screenshots remain under the historical memory results; an additional original COW capture is included in this archive.
- **Later controlled validation:** the unchanged [95-case experiment and full-regression archive](../validation-upgrade/README.md) supplies repeated measurements and completed paging regression. These data have not been replaced by illustrative recaptures.
- **Additional report runs:** current full-page pressure, swap ownership, shared paging, the combined worker/signal case, write-all fork, a Clock hot stream, trace/exec, five signal checks, debugger/lifetime checks, and equal-total-work one/three-worker commands. Current project source is unchanged from the tested source at `7f925b5`; the capture checkout was documentation commit `cae7da6`.
- **Historical eager recapture:** tag `v0.0` at `24d9570`, one CPU and 128 MiB. The allocator and `user/baseline.c` are unchanged. Two serial-output carriage-return additions in the isolated copy make the native console render newlines correctly; the exact presentation-only patch is retained. The source checkout was not edited.

## Reading the comparisons

Historical `forkbench` uses 50 forks and ticks. The later controlled benchmark uses ten forks, microseconds, read-only/write-all modes and a same-source COW switch. Historical `hotcold` and the later four-pattern `pagebench` are different workloads. Their outputs are kept in their respective contexts.

Memory counters accumulate since each guest boot and depend on command order. A program snapshot can differ from a later shell snapshot. Fresh illustrative timings are single executions and are not substituted into the repeated-performance tables.

Every experimental panel is a display crop of an actual image. No output text or numbers have been rewritten inside a capture. PPM-to-PNG conversion is lossless. Full originals are retained; Word stores crop metadata rather than deleting their pixels. The archive manifest hashes every packaged record; the outer manifest hashes the public supplement files.
