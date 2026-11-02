#include "kernel/types.h"
#include "kernel/dbg.h"
#include "user/user.h"
#include "user/thread.h"

static void unused_worker(void *arg) { }

int main(void)
{
  for(int i = 0; i < 8; i++){
    int controller = fork(), status;
    if(controller < 0)
      exit(1);
    if(controller == 0){
      int target = fork();
      if(target < 0)
        exit(1);
      if(target == 0){
        if(dbg_traceme() < 0 || thread_create(unused_worker, 0) >= 0)
          exit(1);
        char *args[] = {"echo", "unreachable orphan target", 0};
        exec("echo", args);
        exit(1);
      }
      if(dbg_wait(target) != DBG_EXEC)
        exit(1);
      // Abandon a stopped target deliberately. Reparent must release it.
      exit(0);
    }
    if(wait(&status) != controller || status != 0){
      printf("dbglife: FAIL\n");
      exit(1);
    }
  }
  sleep(2);
  printf("dbglife: OK (8 abandoned targets and traced thread guard)\n");
  exit(0);
}
