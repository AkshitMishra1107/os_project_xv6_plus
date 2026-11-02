#include "kernel/types.h"
#include "user/user.h"
#include "user/thread.h"

static int data[1000], sums[4], ids[4];
static void
sum_part(void *arg)
{
  int id = (uint64)arg;
  ids[id] = thread_self();
  for(int i = id*250; i < (id+1)*250; i++)
    sums[id] += data[i];
  if(id == 1)
    thread_exit();
}

int
main(void)
{
  int tids[4], total = 0;
  if(thread_self() != 0)
    exit(1);
  for(int i = 0; i < 1000; i++)
    data[i] = i;
  for(int i = 0; i < 4; i++){
    tids[i] = thread_create(sum_part, (void *)(uint64)i);
    if(tids[i] < 1)
      exit(1);
  }
  for(int i = 0; i < 4; i++){
    if(thread_join(tids[i]) != 0 || ids[i] != tids[i])
      exit(1);
    total += sums[i];
  }
  if(total != 499500){
    printf("threadtest: FAIL sum=%d\n", total);
    exit(1);
  }
  printf("threadtest: OK sum = %d (expected 499500)\n", total);
  exit(0);
}
