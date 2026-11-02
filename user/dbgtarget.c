#include "kernel/types.h"
#include "user/user.h"

__attribute__((noinline)) int inner(int x);
__attribute__((noinline)) int middle(int x);
__attribute__((noinline)) int outer(int x);

int
main(void)
{
  printf("dbgtarget: outer(5) = %d\n", outer(5));
  exit(0);
}

__attribute__((noinline)) int
inner(int x)
{
  return x * 2;
}

__attribute__((noinline)) int
middle(int x)
{
  return inner(x) + 1;
}

__attribute__((noinline)) int
outer(int x)
{
  return middle(x) + 1;
}
