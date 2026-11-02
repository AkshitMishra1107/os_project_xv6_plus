# Reproduce validation

Run these commands from the repository root in Ubuntu or WSL, with the normal xv6 toolchain and Python 3 installed. The runner builds isolated copies on the Linux temporary filesystem; it does not change the checked-out kernel or filesystem image.

```bash
python3 tools/validate.py --suite smoke --out validation-output/smoke
python3 tools/validate.py --suite full --out validation-output/full
python3 tools/validate.py --suite bench --out validation-output/bench
```

Choose a new output directory for each run. `full` runs the unmodified xv6 usertests with one CPU for FIFO, Clock and seeded Random, and with three CPUs. Each one-CPU run also audits swap ownership. `smoke` checks the integrated APIs, full-page pressure data and the combined worker/signal/paging workload.

`bench` uses five paired seeds and four access patterns for three replacement policies, five trials of the same-source COW on/off fork benchmark, and five trials per worker count for parallel scaling. Each timing case starts from a fresh guest and pristine image; policy and worker order rotate between trials. The total arithmetic workload and checksum remain equal across worker counts. Guest microseconds are read from the QEMU virt timebase, verified from its generated device tree. These are emulated guest timings, not hardware CPU measurements.

Raw console bytes, individual commands, source hashes, kernel hashes, configuration, JSON/CSV samples and actual virtual-console framebuffer captures are retained. QEMU versions without the hub character device still record raw output and execute checks, but omit framebuffer captures. No web browser is used.

The independent Python reference models replay the same generated traces with a fixed 1700-frame budget for FIFO, Clock, LRU and optimal replacement. This simplified budget is separate from the guest's changing frame availability. The models check the standard Belady example, and the host-compiled pattern negative control detects corruption in each of a page's four disk blocks. Run just these checks with `--suite models`.

`RAM_MB=8` limits xv6's managed physical range; QEMU itself exposes 128 MiB. Disk eviction and the ownership audit require one CPU. Parallel scaling uses three CPUs with eviction disabled. `FORK_COW=0` changes only fork's eager/private-copy selection for the controlled ablation; the default is `FORK_COW=1`.
