#ifndef STDLIB_H
#define STDLIB_H

#define MIN(x,y) ((x < y ? x : y))
#define MAX(x,y) ((x < y ? y : x))

void memcpy(void * dest, void * src, int len);  // Copies <len> bytes from <src> to <dest>
void bzero(void * dest, int len); // Makes <len> bytes of <dest> equal zero
char* itoa (int n); // Decimal integer to ascii

#endif
