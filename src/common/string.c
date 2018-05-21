#include "stdio.h"
#include "string.h"

int strcmp_(const char *str1, const char *str2){
  while(*str1 && *str2 && *str1 == *str2) {str1++; str2++;}
  return *str1 - *str2;
}

char *strchrs_(char *string, const char *searched_chars){
  for(; *string != '\0'; string++){
    for(const char *x = searched_chars; *x != '\0'; x++)
      if(*x == *string) return string;
  }

  return 0;
}

char *strtok_(char *string, const char *delimiters){
  static char *p = 0;

  if(string != 0) p = string;
  if(p == 0) return 0;

  char *n = strchrs_(p, delimiters);
  char *old_p = p;

  if(n != 0){
    p = n+1;
    *n = '\0';
  }
  else
    p = 0;
  
  return old_p;
}

void read_line(char *buf, unsigned int size){
  char *s;
  int c;

  gets(buf, size);

  /*s = strchrs_(buf, '\0');
  if(s != 0) *s = '\0';
  else do { c = getc(); }
  while (c != '\n'&& c != '\r' && c > 0);*/
}
