#include <stdint.h>
#include "atag.h"

#ifndef MEM_H
#define MEM_H

  // Heap

typedef struct heap_segment{
    struct heap_segment * next;
    struct heap_segment * prev;
    uint32_t is_allocated;
    uint32_t segment_size;  // Includes this header
} heap_segment_t;


void * kmalloc(uint32_t size); 
void kfree(void *ptr);         

  // Pages

#define PAGE_SIZE 4096 // TODO: synchronise with the information from atags.
#define KERNEL_HEAP_SIZE (1024*1024)

typedef struct {
  uint8_t allocated: 1;           // This page is allocated to something
  uint8_t kernel_heap_page: 1;         // This page is a part of the kernel
  uint32_t reserved: 30;
} page_flags_t;

typedef struct page {
  uint32_t vaddr_mapped;  // The virtual address that maps to this page   
  page_flags_t flags;
  struct page * next; // Next free page in the linked list
} page_t;

void mem_init(atag_t * atags); // Initializes the memory
void * alloc_page(void);       // Allocates one free memory-page
void free_page(void * ptr);    // Frees one memory-page


// We use a linked list to store free memory slots
page_t * first_free_page;
page_t * last_free_page;
uint32_t num_free_pages;

// Functions of this list

void push_first_free_page(page_t * p); // Adds a page in the beginning of the list
void push_last_free_page(page_t * p);  // Adds a page in the end of the list
page_t * pop_free_page();              // Pops a page from the beginning of the list
page_t * peek_free_page();             // Peeks a page from the beginning of the list

// Printing memory state
void print_curr_memory_state();
void print_gen_memory_state();

#endif
