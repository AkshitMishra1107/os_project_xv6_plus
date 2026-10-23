#include "kernel/types.h"
#include "user/user.h"
#include "kernel/memstat.h"

int
main(void)
{
  struct memstat ms;

  if(memstat(&ms) < 0){
    printf("memstat: failed\n");
    exit(1);
  }

  printf("page faults      %d\n", (int)ms.page_faults);
  printf("lazy allocations %d\n", (int)ms.lazy_allocs);
  printf("cow copies       %d\n", (int)ms.cow_copies);
  printf("swap outs        %d\n", (int)ms.swap_outs);
  printf("swap ins         %d\n", (int)ms.swap_ins);
  printf("free pages       %d\n", (int)ms.free_pages);
  printf("swap slots used  %d\n", (int)ms.swap_slots_used);

  exit(0);
}
