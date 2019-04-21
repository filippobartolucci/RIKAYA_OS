#include "utils.h"

void *memset(void *s, u32 c, int n){
  u32* p=s;
  while(n--)
      *p++ = (unsigned char)c;
  return s;
}

void memcopy(void *dest, void *src, size_t n) { 
   // Typecast src and dest addresses to (char *) 
   char *csrc = (char *)src; 
   char *cdest = (char *)dest; 
  
   // Copy contents of src[] to dest[] 
   for (int i=0; i<n; i++) 
       cdest[i] = csrc[i]; 
} 
