#ifndef USER_SIG_H
#define USER_SIG_H

#include "kernel/sigdefs.h"

// handler is a function address, or SIG_DFL / SIG_IGN
int signal(int sig, uint64 handler);

#endif
