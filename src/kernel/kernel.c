#include <kernel/uart.h>
#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdio.h>
#include <common/stdlib.h>

int n = 226;

void memory_test(){
  int* my_array = kmalloc(n);
  putc (itoa(n)[0]);
  putc (itoa(n)[1]);
  putc (itoa(n)[2]);
  puts(" are allocated\n");
  for(int i = 0; i++; i<n){
    my_array[i]=i;
  }
  kfree (my_array);
  puts("Memory is free\n");
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) // In ARM, the first three parameters of a function are passed through r0, r1 and r2
                                                           // The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
{
  char buf[256];
  (void) r0;
  (void) r1;
  (void) atags;
  
  uart_init();
  
  puts("Initializing Memory Module\n");
  mem_init((atag_t *)atags);
  puts("Running the Memory Test\n");
  memory_test();
  puts("Hello, kernel World!\r\n");
  
  while (1) {
    gets(buf,256);
    puts(buf);
    putc('\n');
  }
}
