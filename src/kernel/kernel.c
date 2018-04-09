#include "uart.h"
#include "framebuffer.h"
#include "mem.h"
#include "atag.h"
#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "blinker.h"

int n = 5, N = 5;


void memory_test(){ // Test memory printing memory states
  int* my_array[N];
  print_gen_memory_state();
  for(int i = 0; i<N; i++){
    my_array[i] = kmalloc(n);
    puts(itoa(n));
    puts(" bytes are allocated\n");
    print_curr_memory_state();
  }
  for(int i = 0; i<N; i++){
    print_curr_memory_state();
    kfree (my_array[i]);
    puts(itoa(n));
    puts(" bytes are free back\n");
  }
  print_curr_memory_state();
}


void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) // In ARM, the first three parameters of a function are passed through r0, r1 and r2
                                                           // The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
{
  char buf[256];
  (void) r0;
  (void) r1;
  (void) atags;

  //for(int i = 0; i < 1000; i++)
  // With console-printing:

  blinker(3); // blinks 3 times, it means the OS has booted

  uart_init();
  puts("Hello, I'm Framboise OS\n");
  
  //puts("Initializing Memory Module\n");
  //mem_init((atag_t *)atags);
  
  //puts("Running the Memory Test\n");
  // memory_test();
  
  // uart_puts("Hello, kernel World!\r\n");

  while(1){   // Echoes all entered characters
    char tmp = uart_getc();
    uart_putc(tmp);
  }
  while (1) {}
}
