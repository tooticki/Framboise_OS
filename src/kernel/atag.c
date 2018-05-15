#include "atag.h"

#define next_tag(t) ((uint32_t *) t) + t->hdr.size;

uint32_t get_mem_size(atag_t * tag) {
  /* TODO: scan atags to find the actual value. */
  return 1024*1024*128;
}


/*

TRY:

uint32_t get_mem_size(atag_t * tag) {
   while (tag->tag != NONE) {
       if (tag->tag == MEM) {
           return tag->mem.size;
       }
       tag = (atag_t *)(((uint32_t *)tag) + tag->tag_size);
   }
   return 0;

}

*/
