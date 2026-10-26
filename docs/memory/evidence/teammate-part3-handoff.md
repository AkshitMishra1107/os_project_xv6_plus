# Part 3 teammate handoff

Current update on 27 October: the shared repository is https://github.com/AkshitMishra1107/os_project_xv6_plus and the saved memory history has been published. Follow the detailed tailored teammate Word manual under docs/guides after the bootstrap is complete. It includes the actual clone URL, the feature/signals-debugger branch routine, prototype adaptation, commit and PR sequence and full source chapters. The earlier offline bundle proposal below is retained only as historical planning; no bundle was required for the successful GitHub publication.

Prepared 27 October 2026. Your assigned area is signals, syscall tracing, and the debugger. The user now proposes creating a shared GitHub repository immediately for simultaneous work, and reports that you already have Ubuntu/build tools. Prefer cloning that shared repository once its URL is ready; the bundle instructions below remain an optional offline fallback. No repository or clone is confirmed yet.

## Shared starting point

The completed memory kernel is tag v0.4, commit 93fa86df3ddda8691cc2c260b373dd58f726620f. It includes lazy allocation, COW fork, memstat, FIFO/Clock swap, and a kernel pipe-copy repair. Full usertests and the final memory demonstrations passed on the owner's machine. Use this saved project base for your preparation.

With GitHub, receive the shared repository URL plus the guides and evidence. If using the offline fallback, you should receive:

- xv6-plus-v0.4.bundle, containing the committed repository history/refs.
- The Part 3 Signals Tracing and Debugger guide.
- The Part 2 Threads and Synchronization guide, for the prerequisite interfaces.
- The report-evidence directory, including environment-versions.txt and two-person-development-plan.md.

The owner creates/verifies a bundle only if that fallback is needed. Source snapshots in it are committed data; guides and sibling evidence are shared separately unless explicitly committed into repository documentation.

## Prerequisite and work schedule

The Part 3 guide explicitly requires completed Part 2 tagged v0.9. Currently v0.4 is finished; v0.9 does not yet exist. Part 3 uses leader/thread metadata, per-thread trapframes, and the ptlock contract from Part 2.

Start now by reading the Part 3 guide, understanding the future Part 2 interfaces, reproducing the memory base, and preparing new modules/tools/tests/design notes on your own branch. A proposed first runnable task is standalone tracing adapted to v0.4, as described below. New signal/debugger preparation can be saved without wiring it into the build until its interfaces exist. Preparation/prototype commits do not claim completion of the final Part 3 runtime milestone.

Coordinate changes to existing proc, trap, VM, syscall and Makefile infrastructure with the Part 2 owner. Full guide-compatible integration/validation follows incorporation of validated v0.9. The standalone tracing prototype is an intentional staged adaptation beyond the guide's stated starting sequence. Preserve the existing pipe fix and memory adaptations.

## Clone from GitHub once the shared URL is supplied

From your chosen project directory, replace SHARED_REPOSITORY_URL with the actual URL provided by the owner:

```bash
git clone SHARED_REPOSITORY_URL xv6-plus
cd xv6-plus
git rev-parse --short v0.4
```

Expect 93fa86d for the common memory base. Then follow the identity/feature-branch steps below. Use either the GitHub clone or the offline clone route, not both into the same destination directory.

## Offline clone fallback

The following commands are for Ubuntu after Git and the RISC-V/QEMU build tools are ready. Choose your own project directory, put the received bundle there, and run from the directory containing it:

```bash
git clone --branch v0.4-swap xv6-plus-v0.4.bundle xv6-plus
cd xv6-plus
git rev-parse --short v0.4
```

The tag should resolve to 93fa86d. This selects the project branch rather than another upstream branch that may also be present in the full-history bundle.

Set your repository-local author identity. Replace both quoted values with your own actual details:

```bash
git config user.name "YOUR NAME"
git config user.email "YOUR EMAIL"
```

Use your own email; a GitHub-linked verified or GitHub-provided noreply email supports later account attribution. Your identity applies to your future commits, not the owner's earlier history.

Create your preparation branch:

```bash
git switch -c feature/signals-debugger v0.4
```

For a GitHub clone, origin points at the shared URL. For a bundle clone, origin initially points to the bundle file and can later be configured to the GitHub URL. Local preparation commits remain in the same clone.

## Proposed first runnable task while Part 2 is developed

Standalone syscall tracing can be adapted to the current single-threaded v0.4 base. This is a proposal for useful executable work in parallel, not an implementation already applied or tested. Before editing, inspect the actual clone's functions and coordinate the shared syscall registry.

- Keep SYS_trace at 29, reserving Part 2's 23 through 28 as agreed. Retain the existing syscall dispatcher range/null checks.
- Add trace_mask and the existing-process fork/reset behavior. Add the real kthread_create/reap_thread inheritance/reset hooks when those functions arrive from Part 2.
- Initially include name-table entries only for calls actually defined in the branch; incorporate the thread/futex entries as their headers arrive. The guide's complete later table assumes those calls already exist.
- Add the trace syscall plumbing and user command using the actual v0.4 argument-helper conventions.
- Validate the prototype using the established CPUS=1 RAM_MB=128 base, trace 32 cat README and inherited fork tracing, with relevant earlier regressions. Use the completed Part 2 configuration for final threaded integration/tests.
- Commit it as a prototype/feature change; the official v0.10 release tag follows integration onto the required stable v0.9 base and the complete trace checks, including thread hooks.

Meanwhile prepare the independent signal/debugger headers, new source modules, user tool and test programs. Their complete guide code depends on Part 2's fields/lock contract. Do not label them runnable before their dependencies and tests are satisfied. This work and the Part 2 owner's branch can be pushed independently; keep main at reviewed, validated milestone states.

## Verify the starting environment

Use environment-versions.txt as the owner's tested environment reference, not as a claim about your own installed versions. If your machine is not set up, report your OS before following installation instructions.

With the toolchain ready, check the starting memory build:

```bash
make clean
make qemu CPUS=1 RAM_MB=128 SWAP_POLICY=1
```

At the xv6 prompt, the existing baseline/lazy/COW tests should work. Record your own actual outputs and resource readings. Keep the established distinction between normal-RAM checks and one-CPU reduced-RAM swap tests. Later thread/signal/debugger demos use the Part 2/3 configuration once that foundation is integrated.

## Your feature milestones after the required base is ready

| Version | Deliverable | Main evidence |
| --- | --- | --- |
| v0.10 | trace syscall and command; inherited trace state | trace 32 cat README and inherited fork tracing |
| v0.11 | Signal state, delivery, handlers, sigreturn and library | sigtest, including delivery and interrupted sleep |
| v0.12 | Debugger kernel protocol | dbgmini stop/resume/exit reasons and cleanup |
| v0.13 | Debugger user tool, ELF symbols, breakpoints and backtraces | dbg dbgtarget session, registers and cleanup |
| v1.0 joint | Combined project integration | finaldemo and complete earlier-feature regressions |

Follow the guide's repetition/resource checks. Keep signal-to-main-thread, single-threaded debugger-target, breakpoint and tracing-mask limits explicit. Both teammates review the debugger kernel/tool protocol.

Proposed syscall allocation remains: existing memstat 22; Part 2 thread/futex calls 23 through 28; trace 29; signals 30 through 33; debugger 34 through 39. Confirm the actual shared headers before wiring.

## Save progress locally

Make small commits on feature/signals-debugger. Review git status and git diff, stage your intended files, and write descriptive commit messages. Save test/configuration output, screenshots, failures and fixes under your Part 3 evidence area. Runtime completion tags are created after implementation and tests, not for untested preparation.

When v0.9 is ready, receive an updated bundle or fetch it from the shared GitHub repository, then merge the agreed base into your preparation branch. The exact update command will be supplied for the actual file/remote available then. Keep your preparation commits and resolve shared-file changes together.

Once GitHub is available, push this branch and open focused PRs. Normal merge commits preserve the individual feature commits in the shared history. Both people use their own identities and review each other's changes.

## Reference

Git bundle transfer and cloning: https://git-scm.com/docs/git-bundle

No source, branch, bundle or repository operation in this handoff has been run by the assistant. The user/teammate perform the commands and share the results.
