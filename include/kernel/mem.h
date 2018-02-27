#include <stdint.h>
#include "kernel/atags.h"

#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096 // TODO: synchronise with atags.

typedef struct {
  uint8_t allocated: 1;           // This page is allocated to something
  uint8_t kernel_page: 1;         // This page is a part of the kernel
  uint32_t reserved: 30;
} page_flags_t;

typedef struct page {
  uint32_t vaddr_mapped;  // The virtual address that maps to this page   
  page_flags_t flags;
  page_t * next_free_page;
} page_t;

void mem_init(atag_t * atags);
void * alloc_page(void);
void free_page(void * ptr);

// Linked list to store free memory slots
typedef 


#endif
