#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "atag.h"

uint32_t get_mem_size(atag_t * tag) {
  return 1024*1024*256;
}


/*
#define next_tag(t) ((uint32_t *) t) + t->hdr.size;

uint32_t get_mem_size(atag_t * t) {
  while (t->hdr.tag != NONE) {
    puts(itoa(t->hdr.tag));
    if (t->hdr.tag == MEM) {
      return t->mem.size;
    }
    t = (atag_t *)next_tag(t);
  }
  return 0;  
}
*/
