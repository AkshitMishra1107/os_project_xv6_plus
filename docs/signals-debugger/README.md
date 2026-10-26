# Signals tracing and debugger records

Owner: teammate. Working branch: `feature/signals-debugger`. Final milestones: v0.10 through v0.13.

Start with the adapted tracing prototype on v0.4. Reserve syscalls 23 through 28 for Part 2 and use 29 for trace. Keep the prototype PR in draft until validated v0.9 is incorporated and the real thread lifecycle hooks are present. Prepare future signal and debugger design notes while Akshit develops the foundation.

No tracing, signal or debugger feature is implemented by the collaboration bootstrap. Future results must be captured on the actual branch and machine.

For each milestone record author, commit, CPU count, RAM_MB, SWAP_POLICY, commands, observed output, repetitions, errors, fixes and before and after memstat. Record the reviewed PR and final release tag. The guide code for copyout_force requires the Part 2 ptlock contract and cannot be pasted unchanged onto v0.4.
