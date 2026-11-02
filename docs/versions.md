# Versions without branch clutter

The latest integrated source is on **main**. A branch is a movable development pointer; a tag names a fixed checkpoint. You can keep versions 1, 2 and later on main using tags without maintaining a separate branch for each version. Temporary branches are useful while two people work simultaneously, then can be deleted after merge without deleting commits.

| Tag or historical checkpoint | Added behavior |
|---|---|
| v0.0 | Verified eager baseline and reservation comparison |
| v0.1 | Lazy allocation |
| v0.2 | COW fork and reap ordering repair |
| v0.3 | memstat syscall and command |
| before-swap | Constrained-memory comparison workload before paging |
| v0.4 | FIFO/Clock swapping and safe pipe copies |
| v0.9 | Integrated thread foundation through kernel threads, futexes, user synchronization and demonstrations |
| v1.0 | Whole-project integration with tracing, signals, debugger, lifecycle repairs and final report |

v0.5–v0.8 and v0.10–v0.13 also occur in implementation commit messages. A message is not automatically a released tag. The original tag names remain unchanged; version labels are identifiers, so v0.10 is not a decimal number equal to v0.1.

Completed remote branches were checked for ancestry in main before removal. A complete local bundle and branch-tip audit preserve the pre-cleanup state. Inherited MIT history and contributor identity are retained. Old timestamps on unchanged upstream files describe their last change; they do not erase project work.
