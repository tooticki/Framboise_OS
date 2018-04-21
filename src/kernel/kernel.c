#include "../common/stdio.h"
#include "../common/stdlib.h"
#include "atag.h"
#include "blinker.h"
#include "uart.h"
#include "mem.h"
#include "interrupts.h"
#include "timer.h"
#include "processes.h"


int n = 5, N = 2;

void memory_test(void){ // Test memory printing memory states
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


void processes_test(void) {
    int i = 0;
    while (1) {
      uart_puts("\ntest ");
      uart_puts(itoa(i++));
      udelay(1000000); // 1 sec
    }
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) // In ARM, the first three parameters of a function are passed through r0, r1 and r2
                                                           // The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
{
  char buf[256];
  (void) r0;
  (void) r1;
  (void) atags;
  int i = 0;

  //for(int i = 0; i < 1000; i++)
  // With console-printing:

  blinker(3); // blinks 3 times, it means the OS has booted

  uart_init();
  puts("Hello, I'm Framboise OS\n");

  /*
  puts("Initializing Memory Module\n");
  mem_init((atag_t *)atags);
  print_gen_memory_state();
  */
  
  //puts("Running the Memory Test\n");
  //memory_test();
  puts("Initializing interrups...\n");
  interrupts_init();
  puts("Done\n");
  puts("Initializing timer...\n");
  timer_init();
  puts("Done\n");
  timer_set(10000000); // 10 sec 
  /*puts("Udelay\n");
  udelay(3000000);
  puts("Udelay is done\n");
  */
  puts("Initializing processes...\n");
  processes_init();
  puts("Done.\n");

  puts("Processes test:\n");
  create_process(processes_test, "TEST", 4);
  while (1) {
    uart_puts("\nmain ");
    uart_puts(itoa(i++));
    udelay(1000000); // 1 sec
  }
  
  while(1){   // Echoes all entered characters
    char tmp = uart_getc();
    uart_putc(tmp);
  }
  while(1);
}
