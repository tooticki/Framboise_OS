#include "kernel/mem.h"
#include "kernel/atag.h"

extern uint8_t __end;
static uint32_t num_pages;

static page_t * all_pages_array;

void mem_init(atag_t * atags) {
    uint32_t mem_size,  page_array_len, kernel_pages, i;

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
        all_pages_array[i].flags.kernel_page = 1;
    }
    // Map the rest of the pages as unallocated, and add them to the free list
    for(; i < num_pages; i++){
        all_pages_array[i].flags.allocated = 0;
        push_last_free_page (&all_pages_array[i]);
    }

}
