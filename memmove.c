
#include <stdio.h>
void* memmove(void* dest, const void* src, size_t n) {
  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;
  
  // 如果目标和源相同，无需复制
  if (d == s) return dest;
  
  // 如果目标在源之前，或者不重叠，从前向后复制
  if (d < s || d >= s + n) {
      for (size_t i = 0; i < n; i++) {
          d[i] = s[i];
      }
  }
  // 否则从后向前复制
  else {
      for (size_t i = n; i > 0; i--) {
          d[i-1] = s[i-1];
      }
  }
  
  return dest;
}