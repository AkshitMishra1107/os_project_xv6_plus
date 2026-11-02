# Working on this project

There is one integrated branch, **main**. Version tags preserve milestones. Completed development and documentation branches are removed only after their tips are part of main; their commits are still in history. A private complete Git bundle was made before the simplification.

## Continue an existing clone

Save your own unfinished work in a local commit first. Then:

```bash
git fetch origin --prune
git switch main
git pull --ff-only origin main
```

If main does not yet exist locally, use `git switch --track origin/main` instead of the switch line. Do not delete a local branch with unmerged work. A missing remote feature branch does not delete your local files or commits.

## Make changes together

For simultaneous work, use one temporary branch for each focused change, such as `fix/signal-test`. Commit with your own identity, push that branch and open a PR to main. Validate shared-kernel changes before merging. Use **Create a merge commit** to retain the original authors' commits; the branch can then be deleted. Do not keep a separate permanent branch for every document revision or version.

For a small sequential documentation edit, agree who is editing, update main first, commit and push main normally. A rejected push means somebody updated it; fetch and integrate rather than force-push. Never replace another person's work or move an existing version tag.

## Authorship and teammate visibility

Nikunj (`nikunjtrehan`) already has write access. His original commits are preserved when the development PR is merged. GitHub associates his recorded commit email with his account; that account mapping was verified before integration. Contributors and profile counts can take time to refresh after merge.

The repository is owned by Akshit, so it does not become a repository owned by Nikunj. Nikunj can open it directly, find it among repositories he contributes to, and pin it on his profile: **Your profile → Customize your pins → select this project → Save pins**. Only Nikunj can change his profile pins and email settings from his own account. A duplicate fork or ownership transfer is not needed for collaboration.

[GitHub contribution rules](https://docs.github.com/en/account-and-profile/reference/profile-contributions-reference) · [Pin a contributed repository](https://docs.github.com/en/account-and-profile/how-tos/profile-customization/pinning-items-to-your-profile)

## Source and evidence

Implementation stays in kernel/ and user/ with Makefile wiring. Mechanism explanations belong in docs/features/. Results belong in docs/results/<feature>/ with outputs/ and screenshots/. Label configuration, source version and whether a result was executed or illustrative. Preserve original captures. Do not upload raw build products, repeated logs, full development manuals or editor screenshots.

Run focused tests and relevant regressions. Normal SMP uses CPUS=3 RAM_MB=128; swap uses CPUS=1 RAM_MB=8 and each replacement policy. Clean between changed definitions. Syscalls 22–39 are assigned; consult kernel/syscall.h. Do not weaken usertests, hide failures, claim POSIX compatibility or infer leak freedom from stable counters.
