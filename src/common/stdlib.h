#ifndef STDLIB_H
#define STDLIB_H

#define MIN(x,y) ((x < y ? x : y))
#define MAX(x,y) ((x < y ? y : x))

void memcpy(void * dest, void * src, int len);  // Copies <len> bytes from <src> to <dest>
void bzero(void * dest, int len); // Makes <len> bytes of <dest> equal zero
unsigned int strlen(const char * s);
char* itoa(int n); // Decimal integer to ascii
int atoi(char* s); // ascii to decimal integer, returns 0 in case of an error

#endif
