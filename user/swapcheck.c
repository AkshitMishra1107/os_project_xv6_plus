#include "kernel/types.h"
#include "kernel/swapaudit.h"
#include "user/user.h"

int
main(void)
{
  struct swapaudit audit;
  if(swapcheck(&audit) < 0){
    printf("swapcheck: audit requires a one-CPU paging build\n");
    exit(1);
  }
  printf("swapcheck: used=%ld references=%ld owners=%ld tables=%ld\n",
         audit.used, audit.references, audit.owners, audit.page_tables);
  printf("swapcheck: orphans=%ld missing=%ld duplicates=%ld invalid=%ld\n",
         audit.orphans, audit.missing, audit.duplicates, audit.invalid);
  for(int i = 0; i < audit.page_tables; i++){
    struct swapowner *o = &audit.owner[i];
    if(o->slots)
      printf("owner pid=%d tid=%d state=%d name=%s slots=%d example=%p\n",
             o->pid, o->tid, o->state, o->name, o->slots, (void *)o->example_va);
  }
  for(int i = 0; i < audit.entries; i++)
    printf("slot=%d pid=%d va=%p\n", audit.entry[i].slot,
           audit.entry[i].pid, (void *)audit.entry[i].va);
  int bad = audit.orphans || audit.missing || audit.duplicates || audit.invalid;
  printf("swapcheck: %s\n", bad ? "FAILED" : "OK");
  exit(bad ? 1 : 0);
}
