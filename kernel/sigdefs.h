#ifndef _SIGDEFS_H_
#define _SIGDEFS_H_

#define NSIG     16
#define SIGINT    2
#define SIGKILL   9
#define SIGUSR1  10
#define SIGUSR2  12
#define SIGALRM  14
#define SIGTERM  15
#define SIG_DFL   0      // default action: terminate the process
#define SIG_IGN   1      // ignore the signal

#endif
