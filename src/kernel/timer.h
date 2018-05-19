#ifndef TIMER_H
#define TIMER_H
#include "peripheral.h"
#include <stdint.h>

#define SYSTEM_TIMER_BASE (SYSTEM_TIMER_OFFSET + PERIPHERAL_BASE)

typedef struct {
    uint8_t timer0_matched: 1;
    uint8_t timer1_matched: 1;
    uint8_t timer2_matched: 1;
    uint8_t timer3_matched: 1;
    uint32_t reserved: 28;
} timer_control_reg_t;

typedef struct {
  timer_control_reg_t control; // controlling timers
  uint32_t counter_low;        // ticks every microsecond (low 32 bits 1.19h)
  uint32_t counter_high;       // ticks every microsecond (high 32 bits)
  uint32_t timer0;             // used by GPU
  uint32_t timer1;             // free to be used
  uint32_t timer2;             // used by GPU
  uint32_t timer3;             // free to be used
} timer_registers_t;

timer_registers_t * timer_regs;


// TODO: maybe they should be static?

void timer_set(uint32_t usecs);
  
void timer_irq_handler(void);

void timer_irq_clearer(void);

void timer_init(void);

void udelay(uint32_t usecs);

#endif
