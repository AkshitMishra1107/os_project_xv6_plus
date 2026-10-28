# Integrated main validation

PR #3 merged at `2e6ae5c91c94d22eb9d6ee1b5f756c4bd9e7a817` and PR #4 at `579f6f2aeaac84c9dda778f793dde5274d70904e` on 28 October 2026. Individual commits and MIT history are preserved. This was owner-authorized integration after local review and testing; no external GitHub approval or CI run is claimed. Nikunj's draft PR #2 remains separate.

The merged kernel/user/build source manifest is `b61837d90722e94e33583db7157c32e7f9e3a449475897ea5301d4665ce88b34`, identical to the four final full-validation configurations already recorded in [validation.json](validation.json). A fresh integrated-main build with 3 CPUs, 128 MiB and policy 1 passed raw thread, futex, library, lifecycle, shared-memory, lazy and COW checks. The fork loop observed eight ticks in this three-CPU run; it is separate from historical one-CPU observations. Free pages stayed at 32300 and slots at zero before/after.

The earlier full usertests and pressure runs were not repeated wholesale for documentation-only integration: their source manifests are identical. [Post-merge focused console output](outputs/integrated-main-smoke.txt) records the new checks. Swapping remains supported with one CPU. The integrated foundation is released as v0.9; the report itself remains a draft for teacher feedback.
