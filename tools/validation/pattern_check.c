#include <stdio.h>
typedef unsigned int uint;
#include "../../user/pagepattern.h"

int main(void)
{
  uint pages[4][PATTERN_PAGE / sizeof(uint)];
  for(int p = 0; p < 4; p++){
    page_fill((char *)pages[p], p, 17);
    if(page_verify((char *)pages[p], p, 17) != 0)
      return 1;
  }
  int detected = 0;
  for(int block = 0; block < 4; block++){
    ((unsigned char *)pages[block])[block * 1024 + 17] ^= 1;
    detected += page_verify((char *)pages[block], block, 17) < 0;
  }
  printf("pattern-check: clean pages passed; corruptions detected=%d/4 blocks\n", detected);
  return detected == 4 ? 0 : 1;
}
