#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include "atag.h"

// Heap

void * kmalloc(uint32_t size); 
void kfree(void *ptr);         

#define KERNEL_HEAP_SIZE (1024*1024)

#define KERNEL_STACK_SIZE PAGE_SIZE
#define IRQ_STACK_SIZE PAGE_SIZE

// Pages

#define PAGE_SIZE 4096

void mem_init(atag_t * atags); // Initializes the memory
void *alloc_page(void);        // Allocates one free memory-page
void free_page(void * ptr);    // Frees one memory-page

// Printing memory state
void print_curr_memory_state();
void print_gen_memory_state();

#endif
