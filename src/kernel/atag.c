#include "atag.h"

#define next_tag(t) ((uint32_t *) t) + t->hdr.size;

uint32_t get_mem_size(atag_t * tag) {
  /* TODO: scan atags to find the actual value. */
  return 1024*1024*128;
}

