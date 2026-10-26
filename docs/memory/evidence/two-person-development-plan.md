# Two person development and GitHub plan

Current update on 27 October: the user supplied https://github.com/AkshitMishra1107/os_project_xv6_plus and explicitly authorized publication plus two detailed role manuals. The saved memory history, five milestone branches, v0.0 through v0.4 tags and before-swap have now been pushed successfully. MIT is retained as upstream and the shared repo is origin. The new main begins at the saved v0.4 source. The tailored guides in repository docs/guides supersede the earlier setup proposals below. Feature branches and documentation publication are being finalized; no kernel feature implementation has been performed by this publishing task. Earlier dated planning statements below remain a historical record.

Prepared 26 October 2026. Proposed allocation and workflow for review; no GitHub repository, invitation, branch, push, or new implementation has been created by the assistant.

Update 27 October: the user confirms wanting the teammate to own signals/tracing/debugger and asks how they can continue while GitHub is postponed. An offline Git-bundle handoff is now proposed in teammate-part3-handoff.md. Bundle creation, transfer, clone, and teammate setup are not yet confirmed.

Later steering on 27 October: the user reports the teammate already has the tools and proposes creating GitHub now so both can clone/develop/push simultaneously. Prefer the shared remote workflow once the actual URL is supplied. A standalone trace prototype adapted to v0.4 is proposed as the teammate's first runnable task; full Part 3 still integrates against v0.9. That staged adaptation is not yet implemented/tested and is not the guide's unmodified starting sequence. No remote/repository action has been performed.

## Starting point

The completed memory implementation is saved locally at commit 93fa86df3ddda8691cc2c260b373dd58f726620f, tag v0.4, branch v0.4-swap. Its full regression and final demonstrations pass. Retain this tag and its history as the shared starting point.

The two new guides are Part 2 Threads and Synchronization and Part 3 Signals, Tracing and a Debugger. Part 2 starts from v0.4; Part 3 explicitly requires completed Part 2 tagged v0.9. Both guides originally describe three-person teams, so the ownership below adapts them to two people.

## Recommended ownership

| Area | Primary owner | Scope |
| --- | --- | --- |
| Completed memory work | You | Existing implementation, evidence, source/tag history, and regressions |
| Part 2 v0.5 through v0.9 | You | Thread-ready memory, kernel threads, futexes, user synchronization library, demos |
| Part 3 v0.10 through v0.13 | Teammate | Syscall tracing, signals, debugger kernel support, debugger user tool |
| Final v1.0 | Both | Combined finaldemo, regression, design notes, contribution table, report, demonstration |

You are the suggested Part 2 owner because its first milestone changes the memory code you just implemented. Each owner writes their own design/results section and reviews the other's shared-kernel changes. The debugger kernel and tool should receive joint review because their protocol spans both sides.

## Dependency and useful parallel work

1. Establish the common saved v0.4 source/history through a shared GitHub clone when available, or through the proposed offline bundle while GitHub is deferred. The later shared main base starts at v0.4.
2. Agree the syscall registry, thread fields, trapframe convention, and page-table lock contract before overlapping kernel edits.
3. You implement and submit Part 2 in milestones: v0.5 memory preparation, v0.6 kernel threads, v0.7 futex, v0.8 library, v0.9 demos.
4. Meanwhile your teammate studies Part 3, reproduces v0.4, reviews your PRs, and prepares new modules/user tools/tests/design notes. A proposed first executable feature is standalone tracing adapted to v0.4; its thread hooks/name-table entries are completed as the Part 2 interfaces arrive. Untested preparation/prototypes are not labelled completed Part 3 releases.
5. Once validated v0.9 is on main, your teammate merges that main base into their branch and completes the Part 3 kernel integration in order: trace, signals, debugger kernel, debugger tool. You review/test those PRs and help with integration and report work.
6. Both run the combined regression before final v1.0.

This is dependency-aware parallel work. The guides do not support treating a complete Part 3 kernel built directly on untouched v0.4 as an independently compatible result. The new standalone-trace proposal is an explicit staged adaptation: reserve agreed numbers, use only present syscall definitions, and add real thread hooks when available. Full signals/debugger integration retains the v0.9 prerequisite.

## Shared interfaces and files

Part 3 uses Part 2's leader, thread bookkeeping, per-thread trapframes, and ptlock. In particular, copyout_force expects the Part 2 pin_user_page lock contract. Coordinate changes to proc.c/proc.h, trap.c, vm.c, exec.c, sysproc.c, sysfile.c, syscall.c/syscall.h, defs.h, user/user.h, user/usys.pl, and Makefile.

During thread-foundation development you lead edits to process/trap/VM infrastructure. During Part 3 integration your teammate leads their feature changes to those files. Shared changes are reviewed together and merged as focused PRs; ownership is coordination, not exclusive permission to contribute.

Preserve the actual v0.4 adaptations, including kernel pipe staging, guarded frame-owner updates, and guarded unmapping. Part 2's mechanical lock-conversion instructions must be reconciled with those additional critical sections and lock acquisition/release ownership. Do not assume the guide's earlier snippets are complete copies of the current saved files.

### Proposed syscall registry from the guides

| Feature | Numbers |
| --- | --- |
| Existing memstat | 22 |
| kthread_create, kthread_exit, kthread_join, kgettid | 23 through 26 |
| futex_wait, futex_wake | 27 and 28 |
| trace | 29 |
| sigaction, sigsend, sigreturn, pause | 30 through 33 |
| dbg_traceme, dbg_wait, dbg_cont, dbg_getregs, dbg_peek, dbg_poke | 34 through 39 |

Validate the current header when implementation begins. Preserve the guide's documented trace-mask limit through syscall 30 unless intentionally redesigned and retested.

## One repository across two devices

Use one shared GitHub repository owned by one account, with the second account invited as a collaborator. Each person has an independent local clone on their own device or drive. Their filesystem paths may differ; their common Git remote connects their work.

Publish the existing xv6-plus repository and its milestone history/tags. The teammate should clone that shared repository. Keep the MIT source remote as an upstream reference when configuring the publishing remote, after inspecting the actual remotes.

Suggested branches:

| Branch | Purpose |
| --- | --- |
| main | Integrated versions that build and pass their milestone checks; initially v0.4 |
| feature/threads-sync | Your Part 2 work, submitted in small milestone PRs |
| feature/signals-debugger | Teammate preparation and later Part 3 integration |

These are proposed names, not branches already created. The same two feature branches can be kept current through normal merges from main; each PR should contain one reviewable milestone. New short-lived milestone branches are also possible after the team is comfortable with the workflow.

## Setup sequence

GitHub can be created later. For work now, the owner can run git bundle create ../xv6-plus-v0.4.bundle --all and verify it, then manually share that bundle plus the guides/evidence. The teammate clones the v0.4-swap branch from the bundle, sets their own repository-local identity, and creates feature/signals-debugger from tag v0.4. Part 3 work remains preparation until the required v0.9 base is incorporated. The bundle transports committed history/refs; the sibling evidence and guide documents are shared separately. See teammate-part3-handoff.md for the recipient steps.

1. Supply repository-owner and teammate GitHub usernames, visibility preference, and an existing shared URL if one already exists.
2. Inspect git remote -v and git branch --list in the existing checkout before changing anything. The current source originated from MIT; publishing must target the team's repository.
3. If creating a new GitHub destination for this existing project, create an empty repository with README/license/gitignore initialization left off. Keep the project's existing history and license files.
4. Establish main at the saved v0.4 base, publish main and the project's milestone refs/tags, and verify the resulting GitHub history. Exact commands depend on actual local branches/remotes and the chosen URL.
5. Invite the teammate as a collaborator; they accept and clone the shared URL using their own account.
6. Configure each clone's own Git author name and a verified GitHub email or its GitHub-provided noreply address. Repository ownership, the person pushing, and commit authorship are separate concepts.
7. Create the two proposed feature branches and begin the phased work above.

No exact publish commands have been issued yet because account/repository details and the current remotes remain to be supplied. Never invent a repository URL or assume a branch name already exists locally.

## Everyday branch and PR routine

After the shared remote and main exist, a developer updates main with git pull --ff-only, switches to their feature branch, makes a small change, reviews git diff, stages the intended files, commits, and pushes their own branch. They open a PR with main as its base.

The other person reviews the change and its test evidence. After any review fixes and required tests, merge using Create a merge commit when preserving each person's individual commits is the goal. A normal merge retains the feature commits and adds an explicit merge point. Squashing combines the feature's commits into one, so it is not the recommended default for this project's progress record.

To bring integrated work into an existing feature branch after committing local edits:

```bash
git fetch origin
git merge origin/main
```

Resolve any conflicts by preserving both intended features, then build and rerun the relevant combined tests. Git text merging is followed by semantic/runtime validation. Retain milestone tags and coordinate tag creation through the integrator after the corresponding version passes on main.

For contribution attribution, use each contributor's actual Git identity. Merging a teammate's branch does not make the original commits the merger's authored work. GitHub associates commits with accounts through the configured commit email.

## Evidence and documentation in the shared repository

The current report-evidence directory is outside the xv6-plus source repository. Plan a separate documentation commit to copy suitable existing material into docs/memory so the teammate can access it through the shared repository.

Suggested ownership:

- docs/memory: existing Part 1 design, source comparisons, measurements, screenshots, failure/fix history.
- docs/threads: your Part 2 implementation notes and evidence.
- docs/signals-debugger: teammate's Part 3 notes and evidence.
- docs/integration: joint combined tests, contribution table, final demo/report material.

Keep each feature owner's results separate while work is parallel, then combine them for the final report. For every PR record what changed, files/interfaces affected, exact build configuration, test output, resource snapshots, and remaining limitations. Record benchmark observations rather than assuming performance improvements.

## Milestone validation modes

| Workload | Configuration |
| --- | --- |
| Thread demos and ordinary signal/debugger work | Three CPUs, 128 MB usable RAM |
| Swap pressure, including combined thread/swap experiments | One CPU, 8 MB usable RAM for the established pressure demos |

Use make clean when changing configuration. The existing swap design is not a supported multi-CPU swapping implementation. Preserve the guides' other limits: main-thread signal delivery, single-threaded debugger targets, no general thread-safe allocator, bounded thread count, and restricted multi-CPU threaded fork/exec behavior.

Before each release, run the relevant milestone tests and earlier-feature regressions. The guides request repeated concurrency/signal tests and before/after memstat. Final v1.0 adds finaldemo and the complete combined checklist. This plan prescribes no autonomous test execution.

## Next information needed

- Your GitHub username and teammate's GitHub username.
- Private or public repository preference; existing shared URL if applicable.
- User-run git remote -v and git branch --list output from the current checkout.

## References

- Local Part 2 guide: prerequisites, roadmap, memory-lock changes, shared thread model, syscall numbers, tests and limitations.
- Local Part 3 guide: required v0.9 base, tracing/signals/debugger milestones, shared fields, syscall numbers, final integration and limits.
- GitHub existing-project publishing: https://docs.github.com/en/migrations/importing-source-code/using-the-command-line-to-import-source-code/adding-locally-hosted-code-to-github
- GitHub collaborator permissions: https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/repository-access-and-collaboration/permission-levels-for-a-personal-account-repository
- GitHub merge history: https://docs.github.com/en/pull-requests/reference/pull-request-merges
- GitHub commit email attribution: https://docs.github.com/en/account-and-profile/how-tos/email-preferences/setting-your-commit-email-address?platform=linux
