# Joint integration and contribution records

## Current baseline

The implemented memory release is v0.4 at 93fa86df3ddda8691cc2c260b373dd58f726620f. Earlier results were supplied by Akshit and preserved in docs/memory. Collaboration documentation does not claim new runtime tests or later features.

## Ownership

| Area | Primary author | Reviewer |
| --- | --- | --- |
| Existing memory implementation and evidence | Akshit | Teammate reproduces starting base |
| v0.5 through v0.9 threads and synchronization | Akshit | Teammate |
| Tracing prototype and v0.10 through v0.13 | Teammate | Akshit |
| Final v1.0 integration and report | Both | Both |

Keep actual commit authorship and reviewed PR links in the final contribution table. Repository ownership or pushing someone else's existing commits does not change authorship.

## Release rules

- Each developer pushes their own feature branch and opens focused PRs to main.
- The other developer reviews shared interfaces and reproduces relevant checks.
- Merge using Create a merge commit, keep both long lived feature branches, and synchronize them through normal merges from main.
- Akshit tags validated main milestones. Do not move published milestone tags.
- The trace prototype remains draft until the v0.9 base and thread hooks are integrated. The release sequence remains v0.5 through v0.13 then v1.0.

## Validation matrix

Record actual outputs and pass or fail for the following modes when their features exist:

| Configuration | Checks |
| --- | --- |
| One CPU 128 MB | Established memory demos and standalone trace prototype |
| Three CPUs 128 MB | Threads, futex, library and synchronization demos; signals and debugger; finaldemo; full usertests |
| One CPU 8 MB FIFO | memhog 12, swapfork, hotcold and resource readings |
| One CPU 8 MB Clock | Same pressure checks for a policy comparison |

Keep fresh boot policy comparisons separate, record repetitions and avoid combining cumulative counters with per workload deltas. Preserve the known one CPU swap restriction and the later main thread signal and single threaded debugger target limits.
