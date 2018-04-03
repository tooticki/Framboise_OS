#include "../kernel/uart.h"
#include "../kernel/framebuffer.h"
#include "stdio.h"
#include "stdlib.h"


void putc(char c){
  uart_putc(c);
}
void puts(const char * s){
  for (int i = 0; s[i] != '\0'; i++)
    putc(s[i]);
}

char getc(void){
  return uart_getc();
}

void gets(char * buf, int buflen){
    char c;
    int i;
    for (i = 0; (c = getc()) != '\r' && i < buflen-1; i++) {
        putc(c);
        buf[i] = c;
    }
    putc('\n');
    if (c == '\n') {
        buf[i] = '\0';
    }
    else
      buf[i] = '\0'; // Not sure
}

