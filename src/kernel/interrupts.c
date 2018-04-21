#include "interrupts.h"
#include "uart.h"
#include "../common/stdlib.h"

static interrupt_registers_t * interrupt_regs;

static interrupt_handler_f handlers[NUM_IRQS];
static interrupt_clearer_f clearers[NUM_IRQS];

// Should be shared with interrputs_vector.S
void move_exceptions_vector_table(void);
extern uint32_t exceptions_vector_table;

void interrupts_init(void) {
  interrupt_regs = (interrupt_registers_t *)INTERRUPTS_PENDING;
  bzero(handlers, sizeof(interrupt_handler_f) * NUM_IRQS);
  bzero(clearers, sizeof(interrupt_clearer_f) * NUM_IRQS);

  // Disable all interrupts
  interrupt_regs->irq_basic_disable = 0xffffffff; 
  interrupt_regs->irq_gpu_disable1 = 0xffffffff;
  interrupt_regs->irq_gpu_disable2 = 0xffffffff;

  move_exceptions_vector_table(); // moves the IVT to address 0 (Interrupt Vector Table)
  ENABLE_INTERRUPTS();
}


// This function is going to be called by the processor.  Needs to check pending interrupts and execute handlers if one is registered

void irq_handler(void) {
  uart_puts("From kernel/interrputs.c irq_handler: we check interrupts\n");
  int j; 
  for (j = 0; j < NUM_IRQS; j++) {
    // If the interrupt is pending and there is a handler, run the handler
    if (IRQ_IS_PENDING(interrupt_regs, j) && (handlers[j] != 0)) {
      uart_puts("From kernel/interrputs.c irq_handler: there is an interrupt\n");
      clearers[j]();
      //      ENABLE_INTERRUPTS(); // TODO: check
      handlers[j]();
      //      DISABLE_INTERRUPTS();
      return;
    }
  }
}

// Not implemented functions

void __attribute__ ((interrupt ("ABORT"))) reset_handler(void) {
  uart_puts("RESET HANDLER\n");
  while(1);
}
void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void) {
  uart_puts("PREFETCH ABORT HANDLER\n");
  while(1);
}
void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void) {
  uart_puts("DATA ABORT HANDLER\n");
  while(1);
}
void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void) {
  uart_puts("UNDEFINED INSTRUCTION HANDLER\n");
  while(1);
}
void __attribute__ ((interrupt ("SWI"))) software_interrupt_handler(void) {
  uart_puts("SWI HANDLER\n");
  while(1);
}
void __attribute__ ((interrupt ("FIQ"))) fast_irq_handler(void) {
  uart_puts("FIQ HANDLER\n");
  while(1);
}


void register_irq_handler(irq_number_t irq_num, interrupt_handler_f handler, interrupt_clearer_f clearer) {
  uint32_t irq_pos;
  if (IRQ_IS_BASIC(irq_num)) {
    irq_pos = irq_num - 64;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_basic_enable |= (1 << irq_pos);
  }
  else if (IRQ_IS_GPU2(irq_num)) {
    irq_pos = irq_num - 32;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_gpu_enable2 |= (1 << irq_pos);
  }
  else if (IRQ_IS_GPU1(irq_num)) {
    irq_pos = irq_num;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_gpu_enable1 |= (1 << irq_pos);
  }
  else {
    uart_puts("Error: cannot register IRQ handler: invalid IRQ number: \n");
  }
}
void unregister_irq_handler(irq_number_t irq_num) { // TODO: find another name
  uint32_t irq_pos;
  if (IRQ_IS_BASIC(irq_num)) {
    irq_pos = irq_num - 64;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    // Setting the disable bit clears the enabled bit
    interrupt_regs->irq_basic_disable |= (1 << irq_pos);
  }
  else if (IRQ_IS_GPU2(irq_num)) {
    irq_pos = irq_num - 32;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    interrupt_regs->irq_gpu_disable2 |= (1 << irq_pos);
  }
  else if (IRQ_IS_GPU1(irq_num)) {
    irq_pos = irq_num;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    interrupt_regs->irq_gpu_disable1 |= (1 << irq_pos);
  }
  else {
    uart_puts("ERROR: CANNOT UNREGISTER IRQ HANDLER: INVALID IRQ NUMBER \n");
  }
}

