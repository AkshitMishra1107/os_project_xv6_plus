#ifndef USER_THREADCHECK_H
#define USER_THREADCHECK_H
#define CHECK(x) do { if(!(x)){ \
  printf("test: FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); \
  exit(1); \
} } while(0)
#endif
