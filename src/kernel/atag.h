/* The bootloader passes parameters to the kernel (before it's loaded)
   to describe the setup it has performed, the size and shape of
   memory in the system and, optionally, other values. This
   information is stored in a list called Atags. */

#ifndef ATAG_H
#define ATAG_H

#include <stdint.h>

typedef enum {
  NONE =      0x00000000, // Empty tag used to end list
  CORE =      0x54410001, // First tag used to start list
  MEM =       0x54410002, // Describes a physical area of memory
  VIDEOTEXT = 0x54410003,
  RAMDISK =   0x54410004,
  INITRD2 =   0x54410005,
  SERIAL =    0x54410006,
  REVISION =  0x54410007,
  VIDEOLFB =  0x54410008,
  CMDLINE =   0x54410009,
} atag_tag_t;

typedef struct {
  uint32_t size;   // legth of tag in words including this header 
  atag_tag_t tag;  // tag value 
} atag_header_t;

typedef struct {
  uint32_t flags;    // bit 0 = read-only
  uint32_t pagesize; // systems page size (4k) 
  uint32_t rootdev;  // root device number
} core_t;

typedef struct {
  uint32_t size;  // size of the area
  uint32_t start; // physical start address 
} mem_t;

typedef struct atag {
  atag_header_t hdr;
  union {
    core_t core;
    mem_t  mem;
  };
} atag_t;

uint32_t get_mem_size(atag_t * tag);

#endif
