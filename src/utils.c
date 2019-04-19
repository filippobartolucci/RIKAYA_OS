#include "utils.h"

void *memset(void *s, u32 c, int n){
  u32* p=s;
  while(n--)
      *p++ = (unsigned char)c;
  return s;
}
