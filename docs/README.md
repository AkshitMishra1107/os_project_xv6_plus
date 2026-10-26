# Project documentation

## Start here

1. Akshit follows [his implementation guide](guides/Akshit%20Threads%20and%20Synchronization%20Guide.docx).
2. The teammate follows [the signals tracing and debugger guide](guides/Teammate%20Signals%20Tracing%20and%20Debugger%20Guide.docx).
3. Both read the shared workflow at the beginning of either manual before editing shared kernel functions.

Each tailored manual contains the complete implementation chapters and source listings from the corresponding supplied guide, with a two person working agreement, exact repository commands, milestone commits, PR and tagging routine, evidence requirements and compatibility notes for the saved v0.4 code. Markdown versions are alongside the Word files for searching and reviewing on GitHub.

## Directories

| Directory | Contents |
| --- | --- |
| `guides` | Two role specific Word manuals and searchable Markdown versions |
| `reference` | The three original supplied guides, preserved unchanged |
| `memory/evidence` | Saved memory screenshots, logs, patches, comparisons and milestone summaries |
| `memory/additional-screenshots` | Earlier screenshots supplied in the local report folder |
| `threads` | Akshit new design notes, transcripts, screenshots and milestone results |
| `signals-debugger` | Teammate new design notes, tracing prototype and Part 3 results |
| `integration` | Joint checks, contribution records and final demonstration notes |

The [evidence manifest](memory/evidence-manifest.json) records the paths, sizes and SHA256 digests of the copied memory records. The original sibling `report-evidence` directory remains a local copy; it is not a separate Git repository. New shared evidence should be added to the appropriate directory inside this repository.

## Original guides

- [Part 1 Memory Management](reference/xv6-Plus%20Part%201%20Memory%20Management%2C%20Complete%20Implementation%20Guide.docx)
- [Part 2 Threads and Synchronization](reference/xv6-Plus%20Part%202%20Threads%20and%20Synchronization%2C%20Complete%20Implementation%20Guide.docx)
- [Part 3 Signals Tracing and Debugger](reference/xv6-Plus%20Part%203%20Signals%2C%20Tracing%20and%20a%20Debugger%2C%20Complete%20Implementation%20Guide.docx)

The original guides use a different team split and local branch and tag routine. Use the tailored manuals for this team. Later source examples still require implementation and runtime validation; example output is not a recorded result.
