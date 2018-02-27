#include <stddef.h>
#include <stdint.h>
#include <kernel/uart.h>

// a register is 0x20000000 + peripheral base + register offset

static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
    return *(volatile uint32_t*)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : "=r"(count): [count]"0"(count) : "cc");
}


void uart_init()
{
    mmio_write(UART0_CR, 0x00000000); // disables all aspects of the UART hardwar; UART0_CR is the UART’s Control Register.

    // disables pins 14 and 15 of the GPIO
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    mmio_write(GPPUDCLK0, 0x00000000);

    mmio_write(UART0_ICR, 0x7FF); //  sets all flags in the Interrupt Clear Register.

    // tries to get a baud rate (bits-per-second that can go across the serial port) of 115200.
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6)); // Line control register: the UART hardware will hold data in an 8 item deep FIFO; data sent or received will have 8-bit long words

    mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10)); // disables all interrupts from the UART by writing a one to the relevent bits of the Interrupt Mask Set Clear register

    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9)); //  enables the UART hardware; enables the ability to receive data; enables the ability to transmit data
}

void uart_putc(unsigned char c) // wraps up putc in a loop so we can write whole strings
{
  while ( mmio_read(UART0_FR) & (1 << 5) ) { } // Flags Register
  mmio_write(UART0_DR, c); // Data Register
} 

unsigned char uart_getc()
{
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}

void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i ++)
        uart_putc((unsigned char)str[i]);
}
