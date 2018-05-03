/* Magic numbers for accessing the hardware */

#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#define PERIPHERAL_BASE 0x20000000
#define PERIPHERAL_LENGTH 0x01000000

#define SYSTEM_TIMER_OFFSET 0x3000 // TODO: check me
#define INTERRUPTS_OFFSET 0xB000   // TODO: check me

#define MAILBOX_OFFSET 0xB880
#define UART0_OFFSET 0x201000
#define GPIO_OFFSET 0x200000

#define GPIO_GPFSEL0    0
#define GPIO_GPFSEL1    1
#define GPIO_GPFSEL2    2
#define GPIO_GPFSEL3    3
#define GPIO_GPFSEL4    4
#define GPIO_GPFSEL5    5

#define GPIO_GPSET0     7
#define GPIO_GPSET1     8

#define GPIO_GPCLR0     10
#define GPIO_GPCLR1     11

#define L_GPIO_BASE     0x20200000UL
#define LED_GPFSEL      GPIO_GPFSEL4
#define LED_GPFBIT      21

#define LED_GPSET       GPIO_GPSET1
#define LED_GPCLR       GPIO_GPCLR1
#define LED_GPIO_BIT    15

#endif
