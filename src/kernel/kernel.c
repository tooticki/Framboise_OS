#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "atag.h"
#include "blinker.h"
#include "uart.h"
#include "mem.h"
#include "interrupts.h"
#include "timer.h"
#include "processes.h"
#include "priority_scheduler.h"
#include "syscalls.h"
#include "minishell.h"


// Test memory printing memory states
void memory_test(void){ 
  static int n = 5, N = 2;
  int* my_array[5];

  print_gen_memory_state();

  for(int i = 0; i<N; i++){
    my_array[i] = kmalloc(n);
    puts(itoa(n));
    puts(" bytes are allocated\n");
    print_curr_memory_state();
  }

  for(int i = 0; i<N; i++){
    print_curr_memory_state();
    kfree(my_array[i]);
    puts(itoa(n));
    puts(" bytes are freed back\n");
  }

  print_curr_memory_state();
}

void processes_test(void) {
  int i = 0;
  while (i<3) {
    //current_process_report();
    puts(itoa(i++));
    puts("\n");
    udelay(1000000); // 1 sec
  }
}


void main(void);

// In ARM, the first three parameters of a function are passed through r0, r1 and r2
// The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
  (void) r0; // Avoid warning for r0 not being used
  (void) r1; // Ditto 
  
  uart_init();

  puts("Hello, I'm Framboise OS\n");

  puts("Initializing interrups...\n");
  interrupts_init();
  puts("Done\n");
  
  puts("Initializing timer...\n");
  timer_init();
  puts("Done\n");

  puts("Affable blinking...\n");
  blinker(3); // blinks 3 times, it means the OS has booted
  puts("Done\n");

  puts("Initializing Memory Manager...\n");
  mem_init((atag_t *)atags);
  puts("Done\n");
  
  puts("Initializing system calls...\n");
  syscalls_init();
  puts("Done\n");

  puts("Initializing processes...\n");
  processes_init(PRIORITY);
  puts("Done.\n");

  main();

  // Should never be reached: main doesn't return
  // In case this is reached anyway, we'll jump back to boot.S and the code
  // there shall halt the computer.
  puts("Oops, something went wrong. Halting now...\n");
}

void echo_chars(void)
{
  while(1){
    char tmp = getc();
    putc(tmp);
  }
}

void main(void)
{
  //  user_puts("PRINT ME\n");
  //  user_puts("PRINT ME AGAIN\n");
  // puts("Syscalls test done\n");
  run_shell();
  puts("Oops, something went wrong. Just echoing characters now...\n");
  // Echoes all entered characters... should never be reached
  echo_chars();
}
