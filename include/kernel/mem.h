#include <stdint.h>
#include "kernel/atag.h"

#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096 // TODO: synchronise with the information from atags.

typedef struct {
  uint8_t allocated: 1;           // This page is allocated to something
  uint8_t kernel_page: 1;         // This page is a part of the kernel
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
uint32_t num_free_pages = 0;

// Functions of this list

void push_first_free_page(page_t * p){ // Adds a page in the beginning of the list
  if(num_free_pages == 0){
    first_free_page = p;
    last_free_page = p;
  }
  else{
    p->next = first_free_page;
    first_free_page = p;
  }
  num_free_pages ++;
}

void push_last_free_page(page_t * p){ // Adds a page in the end of the list
  if(num_free_pages == 0){
    first_free_page = p;
    last_free_page = p;
  }
  else{
    last_free_page->next = p;
    last_free_page = p;
  }
  num_free_pages ++;
}

page_t * pop_free_page(){ // Pops a page from the beginning of the list
    if(num_free_pages == 0)
    return 0;
  page_t * first_page = first_free_page;
  page_t * new_first_page = first_free_page->next;
  first_free_page = new_first_page;
  num_free_pages --;
  return first_page;
}

page_t * peek_free_page(){ // Peeks a page from the beginning of the list
  return first_free_page;
}

#endif
