#include "mem.h"
#include <limits.h>
#include "atag.h"
#include "../common/stdlib.h"
#include "../common/stdio.h"

// Heap

typedef struct heap_segment{
    struct heap_segment * next;
    struct heap_segment * prev;
    uint32_t is_allocated;
    uint32_t segment_size;  // Includes this header
} heap_segment_t;

#define head_size sizeof(heap_segment_t)

static heap_segment_t * first_heap_segment;

// Called from mem_init
static
void heap_init(uint32_t heap_start) {
  first_heap_segment = (heap_segment_t *) heap_start;
  bzero(first_heap_segment, head_size);
  first_heap_segment->segment_size = KERNEL_HEAP_SIZE;
}

void * kmalloc(uint32_t size) {
  heap_segment_t * curr, *tmp, *new_next, *best = 0;
  unsigned int currdiff, bestdiff =  UINT_MAX; // Max unsigned int

  size+=head_size;                     // Add the segment header size to the needed ammount of memory
  if(size%16 !=0) size+=(16-size%16);  // Complete the size to be divisible by 16

  // Find the segment with the closest size
  if(first_heap_segment == 0) return 0; // No free memory
  
  for(curr = first_heap_segment; curr != 0; curr = curr->next){
    currdiff = curr->segment_size - size;
    if(!curr->is_allocated && currdiff > 0 && currdiff < bestdiff){
      best = curr;
      bestdiff = currdiff;
    }
  }

  if(best == 0){
    return 0; // Didn't find a segment big enough
  }
  
  // If the best found segment is too large (exceeds the needed size by > head_size) , split it in two
  if(bestdiff > 2*head_size){
    tmp = best->next;
    new_next = ((void*)(best)) + size;
    bzero(new_next, head_size);
    best->next = new_next;
    best->segment_size = size;
    new_next->next = tmp;
    new_next->prev = best;
    new_next->is_allocated = 0;
    new_next->segment_size = bestdiff;
  }
  best->is_allocated = 1;
  return best + 1;
}

void kfree(void *ptr) {
    heap_segment_t * seg = ptr - head_size;
    seg->is_allocated = 0;

    // Try to coalesce segements to the left
    while(seg->prev != 0 && !seg->prev->is_allocated) {
        seg->prev->next = seg->next;
        seg->prev->segment_size += seg->segment_size;
        seg = seg->prev;
    }
    // Try to coalesce segments to the right
    while(seg->next != 0 && !seg->next->is_allocated) {
        seg->segment_size += seg->next->segment_size;
        seg->next = seg->next->next;
    }
}

// Pages (hook for future virtual memory implementation)

// We implement a list of pages and functions to manipulate them.

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


static page_t * first_free_page;
static page_t * last_free_page;
static uint32_t num_free_pages;

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
    
// From boot.S
extern uint8_t __end;

static uint32_t num_pages;
static page_t * all_pages_array;

void * alloc_page(void) {
    page_t * page;
    void * page_mem;
    if (num_free_pages == 0)
        return 0;

    // Get a free page
    page = pop_free_page();
    page->flags.kernel_heap_page = 1;
    page->flags.allocated = 1;

    // Get the address the physical page metadata refers to
    page_mem = (void *)((page - all_pages_array) * PAGE_SIZE);

    // Zero out the page
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void * ptr) {
    page_t * page;

    // Get page metadata from the physical address
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // Mark the page as free
    page->flags.allocated = 0;
    push_last_free_page(page);
}

// Memory initialization

void mem_init(atag_t * atags) {
  uint32_t mem_size, page_array_len, page_array_end, kernel_pages, i;
  
  // Get the total number of pages
  mem_size = get_mem_size(atags);
  num_pages = mem_size / PAGE_SIZE;
  
  // Allocate space for all those pages' metadata.  Start this block just after the kernel image is finished
  page_array_len = sizeof(page_t) * num_pages;
  all_pages_array = (page_t *)&__end;
  bzero(all_pages_array, page_array_len);

  // Iterate over all pages and mark them with the appropriate flags
  // Start with kernel pages
  kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
  for (i = 0; i < kernel_pages; i++) {
    all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
    all_pages_array[i].flags.allocated = 1;
    all_pages_array[i].flags.kernel_heap_page = 1;
  }
  
  // Reserve 1 MB for the kernel heap
  for(; i < kernel_pages + (KERNEL_HEAP_SIZE / PAGE_SIZE); i++){
    all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
    all_pages_array[i].flags.allocated = 1;
    all_pages_array[i].flags.kernel_heap_page = 1;
  }
    
  // Map the rest of the pages as unallocated, and add them to the free list
  for(; i < num_pages; i++){
    all_pages_array[i].flags.allocated = 0;
    push_last_free_page (&all_pages_array[i]);
  }
  
  // Initialize the heap
  page_array_end = (uint32_t)&__end + page_array_len;
  heap_init(page_array_end);
}

 // Printing the state of memory for testing

void print_gen_memory_state(){
  // General information
  puts("\n*** Memory information\n\n");
  puts("  Size of the heap: ");
  puts(itoa(KERNEL_HEAP_SIZE));
  puts(" bytes\n");
  puts("  Size of a page: ");
  puts(itoa(PAGE_SIZE));
  puts(" bytes\n");
  puts("  Header size for a heap segment: ");
  puts(itoa(head_size));
  puts(" bytes\n");
  puts("\n***End of memory informaiton\n\n");
}

void print_curr_memory_state(){
  // Heap memory
  heap_segment_t * curr;
  puts("\n*** Current memory state\n\n");
  
  puts("  Heap segments:\n");
  for(curr = first_heap_segment; curr != 0; curr = curr->next){
    if(curr->is_allocated)
      puts ("  - Allocated   ");
    else
      puts ("  - Free        ");    
    puts("segment of size ");
    puts(itoa(curr->segment_size));
    puts(" bytes \n");
  }
  // Free pages
  puts("\n*** End of current memory state\n\n");
}

