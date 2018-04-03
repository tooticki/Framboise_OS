#include "uart.h"
#include "framebuffer.h"
#include "mem.h"
#include "atag.h"
#include "../common/stdio.h"
#include "../common/stdlib.h"
#include "blinker.h"

int n = 5, N = 5;


/** blinker */
int blinker(void)
{
    /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    gpio = (unsigned int*)GPIO_BASE;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
           for plus models )*/
        gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    }
}



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

  blinker();
  //uart_init();
  //while(1){
  //  puts("a\n");
  //}
  //framebuffer_init();
  //orange_screen();


  //puts("Initializing Memory Module\n");
  //mem_init((atag_t *)atags);
  
  //puts("Running the Memory Test\n");
  // memory_test();
  
  // uart_puts("Hello, kernel World!\r\n");
  
  /*while (1) {
    // gets(buf,256);
    // puts(buf);
    uart_putc('a');
    uart_putc('\n');
    }*/
}
