#include "blinker.h"
#include "peripheral.h"
#include "timer.h"

void blinker(unsigned int n)
{
    /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    volatile unsigned *gpio = (unsigned int*)L_GPIO_BASE;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Blink n times in a row*/
    for(int i = 0; i < n; i++)
    {
      udelay(500000);

	/* Set the LED GPIO pin high (Turn OK LED off for original Pi, and on
           for plus models) */
      gpio[LED_GPSET] = (1 << LED_GPIO_BIT);

      udelay(500000);

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
      gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);	
  }
}

