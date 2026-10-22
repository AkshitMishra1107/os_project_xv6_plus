#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  char *p = sbrk(200 * 1024 * 1024);

  if(p == (char*)-1)
    printf("baseline: sbrk(200MB) failed (expected on stock xv6)\n");
  else
    printf("baseline: sbrk(200MB) succeeded\n");

  exit(0);
}
