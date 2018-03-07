#include <common/stdlib.h>

void memcpy(void * dest, void * src, int len) {
    char * d = dest, * s = src;
    while (len-- > 0) {
        *d++ = *s++;
    }
}

void bzero(void * dest, int len) {
    char * d = dest;
    while (len-- > 0) {
        *d++ = 0;
    }
}

char* itoa (int n){ 
  static char intbuf[16];
  int i = 0, is_neg = 0, j = 0, tmp;
  if (n == 0){
    intbuf[0] = '0';
    intbuf[1] = '\0';
    return intbuf;
  }
  if (n < 0){
    is_neg = 1;\
    n = -n;
  }
  while(n >= 1 && i<16){
    intbuf[i] = '0'+ n%10;
    i++;
    n /= 10;
  }

  if(is_neg)
    intbuf[i++] = '-';

  intbuf[i] = '\0';
  i--;
  
  while (j < i) {
    tmp = intbuf[j];
    intbuf[j] = intbuf[i];
    intbuf[i] = tmp;
    j++;
    i--;
    }
  
  return intbuf;
}
