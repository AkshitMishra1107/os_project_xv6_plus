#ifndef XV6_PAGEPATTERN_H
#define XV6_PAGEPATTERN_H
#define PATTERN_PAGE 4096
static uint
page_pattern(uint page, uint word, uint seed)
{
  uint x = (page + 1) * 2654435761U ^ (word + 1) * 2246822519U ^ seed;
  x ^= x >> 16;
  return x * 3266489917U;
}
static void
page_fill(char *p, uint page, uint seed)
{
  uint *words = (uint *)p;
  for(uint j = 0; j < PATTERN_PAGE / sizeof(uint); j++)
    words[j] = page_pattern(page, j, seed);
}
static int
page_verify(char *p, uint page, uint seed)
{
  uint *words = (uint *)p;
  for(uint j = 0; j < PATTERN_PAGE / sizeof(uint); j++)
    if(words[j] != page_pattern(page, j, seed))
      return -1;
  return 0;
}
#endif
