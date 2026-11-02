# Historical signals tracing and debugger evidence

These supplied development-branch captures demonstrate standalone tracing, signals and debugger operations. They precede integration with the real kernel thread library. The earlier finaldemo used a temporary sequential wrapper, so its screenshot does **not** establish thread concurrency or leak freedom. Use the [new combined validation](../final-integration/README.md) for integrated claims.

| Capability | Earlier baseline | Development observation |
|---|---|---|
| trace | Command absent | PID/name/return logging and fork inheritance |
| sigtest | Command absent | Five signal scenarios pass |
| dbgmini | Command absent | Exec stop, register read, continue and exit reason |
| dbg dbgtarget | No in-guest debugger | Known function breakpoints and symbolized frames |

Selected original screenshots: [trace](screenshots/01-trace-read-test.png), [signals](screenshots/06-signals-sigtest.png), [debugger](screenshots/10-dbg-tool-session.png). Recorded outputs remain under outputs/. They are retained as supplied observations, not presented as newly executed final validation. The code does not provide a general signal mask API, full POSIX semantics, general instruction stepping or all-configuration certification.
