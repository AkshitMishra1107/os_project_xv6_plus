# Threads and synchronization records

Owner: Akshit. Working branch: `feature/threads-sync`. Milestones: v0.5 through v0.9.

No Part 2 feature is implemented by the collaboration bootstrap. Start with the role manual and reproduce the v0.4 memory base before changing lock ownership.

For each milestone add a dated transcript, focused screenshots and release notes recording the commit ID, configuration, exact commands, observed results, repeat count, failures and repairs. Include the shared function changes and before and after memory statistics. After review, record the PR URL and release tag.

Keep a lock ownership table while converting the memory code. In particular, review existing tracking and unmapping interrupt guards, the kfree boot path, the pin_user_page return contract, the pipe staging repair and cleanup before wait status copyout.
