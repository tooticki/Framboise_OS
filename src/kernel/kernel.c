#include <stddef.h>
#include <stdint.h>
#include <kernel/uart.h>

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) // In ARM, the first three parameters of a function are passed through r0, r1 and r2
                                                           // The bootloaderplaces some information about the hardware/command line (used to run the kernel) in memory. This information is called atags
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart_init();
    uart_puts("Hello, kernel World!\r\n");

    while (1) {
        uart_putc(uart_getc());
        uart_putc('\n');
    }
}
