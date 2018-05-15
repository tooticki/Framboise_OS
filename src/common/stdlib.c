#include "stdio.h"
#include "stdlib.h"


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

unsigned int strlen(const char * s) {
  unsigned int r = 0;
  while(s[r] != '\0')
    r++;
  return r;
}

char* itoa(int n){ 
  static char intbuf[20];
  int i = 0, is_neg = 0, j = 0, tmp;
  if(n == 0){
    intbuf[0] = '0';
    intbuf[1] = '\0';
    return intbuf;
  }
  if(n < 0){
    is_neg = 1;
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
  
  while(j < i) {
    tmp = intbuf[j];
    intbuf[j] = intbuf[i];
    intbuf[i] = tmp;
    j++;
    i--;
    }
  
  return intbuf;
}

int atoi(char* s){
  int i = 0, sign = 1, n = 0;
  if(s[0] == '-'){
    sign = -1;
    i++;
  }
  while(s[i] >= '0' && s[i] <= '9'){
    n *= 10;
    n += s[i]-'0';
    i++;
  }
  return sign * n;
}
