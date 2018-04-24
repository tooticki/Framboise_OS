#include "../common/stdio.h"
#include "../common/stdlib.h"
#include "atag.h"
#include "blinker.h"
#include "uart.h"
#include "mem.h"
#include "interrupts.h"
#include "timer.h"
#include "syscalls.h"
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
      uart_puts("\nprocess 1 says: ");
      uart_puts(itoa(i++));
      udelay(500000); // 0.5 secs
    }
}

void main(void);

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) // In ARM, the first three parameters of a function are passed through r0, r1 and r2
                                                           // The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
{
  (void) r0; // Avoid warning for r0 not being used
  (void) r1; // Ditto r1

  blinker(3); // blinks 3 times, it means the OS has booted

  uart_init();

  puts("Hello, I'm Framboise OS\n");

  puts("Initializing Memory Manager...\n");
  mem_init((atag_t *)atags);
  puts("Done\n");

  puts("Initializing system calls...\n");
  syscalls_init();
  puts("Done\n");

  puts("Initializing interrups...\n");
  interrupts_init();
  puts("Done\n");

  puts("Initializing timer...\n");
  timer_init();
  puts("Done\n");

#if 0
  puts("Udelay 3,000,000 microseconds...\n");
  udelay(3000000);
  puts("Done\n");
#endif

  puts("Initializing processes...\n");
  processes_init();
  puts("Done.\n");

  puts("Creating a new process...\n");
  create_process(processes_test, "TEST", 4);
  puts("Done.\n");

  // Should never return...
  main();

  while(1) {}
}

void main(void)
{
  int i = 0;

  puts("We are now in process 0. Press any key to start.");
  getc();

  // Just increments a counter ad vitam aeternam
  while (1) {
    uart_puts("\nSheumeugafoo...");
    user_puts("\nprocess 0 says: ");
    uart_puts(itoa(i++));
    udelay(500000); // 0.5 secs
  }

  // Echoes all entered characters... should never be reached
  while(1){
    char tmp = uart_getc();
    uart_putc(tmp);
  }
}
