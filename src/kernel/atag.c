#include "atag.h"

#define next_tag(t) ((uint32_t *) t) + t->hdr.size;


uint32_t get_mem_size(atag_t * tag) {
  // For the VM:
  return 1024*1024*128;

  // Otherwise:
  /*
  while (tag->hdr.tag != NONE) {
       if (tag->hdr.tag == MEM) {
           return tag->mem.size;
       }
       tag = next_tag(tag);
   }
   return 0;
  */
}

