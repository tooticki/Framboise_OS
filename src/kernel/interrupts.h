#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "peripheral.h"
#include <stdint.h>

/* How many IRQs are there? */
#define NUM_IRQS 72

/* Testing which kind of IRQ some number is. */
#define IRQ_IS_BASIC(x) ((x) >= 64 )
#define IRQ_IS_GPU2(x) ((x) >= 32 && (x) < 64 )
#define IRQ_IS_GPU1(x) ((x) < 32 )

/* Hardware magic structure describing the state of IRQ. */
typedef struct {
  uint32_t irq_basic_pending;
  uint32_t irq_gpu_pending1;
  uint32_t irq_gpu_pending2;
  uint32_t fiq_control;
  uint32_t irq_gpu_enable1;
  uint32_t irq_gpu_enable2;
  uint32_t irq_basic_enable;
  uint32_t irq_gpu_disable1;
  uint32_t irq_gpu_disable2;
  uint32_t irq_basic_disable;
} interrupt_registers_t;

inline
int IRQ_IS_PENDING(interrupt_registers_t *regs, unsigned int num)
{
  return
    (IRQ_IS_BASIC(num) && ((1 << (num-64)) & regs->irq_basic_pending)) ||
    (IRQ_IS_GPU1(num)  && ((1 << (num)) & regs->irq_gpu_pending1)) ||
    (IRQ_IS_GPU2(num)  && ((1 << (num-32)) & regs->irq_gpu_pending2)) ||
    0;
}

inline
int INTERRUPTS_ENABLED(void) {
  int res;
  __asm__ __volatile__("mrs %[res], CPSR": [res] "=r" (res)::);
  return ((res >> 7) & 1) == 0;
}

inline
void ENABLE_INTERRUPTS(void) {
  if (!INTERRUPTS_ENABLED()) {
    __asm__ __volatile__("cpsie i"); // cps = Change Processor State; ie = Interrupts Enable ; i is for IRQs (f is for FIQs)
  }
}

inline
void DISABLE_INTERRUPTS(void) {
  if (INTERRUPTS_ENABLED()) {
    __asm__ __volatile__("cpsid i"); // cps = Change Processor State; id = Interrupts Disable ; i is for IRQs (f is for FIQs)
  }
}

typedef void (*interrupt_handler_f)(void);
typedef void (*interrupt_clearer_f)(void);

typedef enum {
  SYSTEM_TIMER_1 = 1,
  USB_CONTROLER = 9,
  ARM_TIMER = 64
} irq_number_t;

void interrupts_init(void);
void register_irq_handler(irq_number_t irq_num, interrupt_handler_f handler, interrupt_clearer_f clearer);
void unregister_irq_handler(irq_number_t irq_num);

#endif
