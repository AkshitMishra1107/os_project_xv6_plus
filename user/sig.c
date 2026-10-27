#include "kernel/types.h"
#include "user/user.h"
#include "user/sig.h"

// When a handler returns, it "returns" here: the kernel put this address
// in the return-address register before running the handler.
static void
sig_restorer(void)
{
  sigreturn();
}

int
signal(int sig, uint64 handler)
{
  return sigaction(sig, handler, (uint64)sig_restorer);
}
