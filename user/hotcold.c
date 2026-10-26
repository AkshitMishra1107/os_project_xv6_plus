#include "kernel/types.h"
#include "user/user.h"
#include "kernel/memstat.h"

#define PAGE 4096

int
main(void)
{
  int mb = 12;
  int hot = 512;
  int npages = mb * 256;

  volatile char *base = sbrk(mb * 1024 * 1024);

  if(base == (char *)-1){
    printf("hotcold: sbrk failed\n");
    exit(1);
  }

  printf("hotcold: 12 MB, 3 rounds\n");

  // Initialize every page.
  for(int i = 0; i < npages; i++)
    base[i * PAGE] = 1;

  struct memstat before, after;

  if(memstat(&before) < 0){
    printf("hotcold: initial memstat failed\n");
    exit(1);
  }

  for(int round = 0; round < 3; round++){
    for(int i = hot; i < npages; i++){
      // Stream through cold pages.
      base[i * PAGE] += 1;

      // Revisit selected pages in the hot region.
      if(i % 20 == 0){
        for(int h = 0; h < hot; h += 8)
          base[h * PAGE] += 1;
      }
    }
  }

  if(memstat(&after) < 0){
    printf("hotcold: final memstat failed\n");
    exit(1);
  }

  printf("hotcold: swap ins %d, swap outs %d\n",
         (int)(after.swap_ins - before.swap_ins),
         (int)(after.swap_outs - before.swap_outs));

  exit(0);
}
