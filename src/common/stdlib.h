#ifndef STDLIB_H
#define STDLIB_H

void memcpy(void * dest, void * src, int len);  // Copies <len> bytes from <src> to <dest>
void bzero(void * dest, int len); // Makes <len> bytes of <dest> equal zero
char* itoa (int n); // Decimal integer to ascii

#endif
