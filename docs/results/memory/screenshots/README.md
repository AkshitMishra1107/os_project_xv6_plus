# Selected memory screenshots

These files are unchanged copies of the original captured screenshots. The original names below refer to the preserved local development archive. Historical comparisons use the checkpoint described, rather than attributing every experiment to the final release.

| Public filename | Original capture | What it shows |
| --- | --- | --- |
| [lazy-allocation.png](lazy-allocation.png) | `07-lazy-allocation-results.png` | Lazy reservation and sparse-touch success at v0.1 |
| [fork-before-cow.png](fork-before-cow.png) | `09-before-cow.png` | Large-process fork failure and 38-tick benchmark before COW |
| [cow-validation.png](cow-validation.png) | `39-v0.4-baseline-lazy-cow-demo.png` | Baseline reservation, lazy and COW demonstrations with the final memory implementation |
| [fork-benchmark.png](fork-benchmark.png) | `40-v0.4-final-forkbench-and-resource-recovery.png` | Final integrated fork benchmark of 6 ticks and memory statistics |
| [no-swap-pressure.png](no-swap-pressure.png) | `25-no-swap-pressure-baseline.png` | 4 MB control succeeds and 12 MB workload terminates without swap |
| [swap-pressure.png](swap-pressure.png) | `28-fifo-swap-memhog-results.png` | 12 MB workload verifies 3072 pages with zero bad pages and exit status 0 |
| [swap-fork.png](swap-fork.png) | `30-fifo-swapfork-results.png` | COW and swap interaction checks pass |
| [fifo-hotcold.png](fifo-hotcold.png) | `31-fifo-hotcold-results.png` | Recorded FIFO hot/cold measured phase |
| [clock-hotcold.png](clock-hotcold.png) | `32-clock-hotcold-results-and-command-retry.png` | Recorded Clock hot/cold measured phase; unrelated command retry is visible |
| [regression-tests.png](regression-tests.png) | `38-v0.4-full-usertests-pass-and-memstat.png` | Final full regression pass and memstat after the pipe repair |
| [memstat-observation.png](memstat-observation.png) | `20-v0.3-memstat-results.png` | v0.3 cumulative counters and free-page recovery |
