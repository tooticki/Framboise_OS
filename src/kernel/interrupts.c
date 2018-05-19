#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "syscalls.h"
#include "interrupts.h"

static interrupt_registers_t * interrupt_regs = (interrupt_registers_t *)INTERRUPTS_PENDING;
static interrupt_handler_f handlers[NUM_IRQS];
static interrupt_clearer_f clearers[NUM_IRQS];

/* You never know when you'll need to do nothing. */
void do_nothing(void) {
}

/* These things live in interrputs_vector.S */
void install_exceptions_vector_table(void);
extern uint32_t exceptions_vector_table;

void interrupts_init(void) {
  unsigned int i;
  for(i = 0; i < NUM_IRQS; i++)
  {
    handlers[i] = &do_nothing;
    clearers[i] = &do_nothing;
  }

  /* Disable all interrupts */
  interrupt_regs->irq_basic_disable = 0xffffffff; // is 1111...111 (32 times) in binary
  interrupt_regs->irq_gpu_disable1 = 0xffffffff;
  interrupt_regs->irq_gpu_disable2 = 0xffffffff;

  install_exceptions_vector_table(); 
  ENABLE_INTERRUPTS();
}

/* This function is going to be called by the processor on each
   hardware interrupt (see interrupts_vector.S to see why).
   It checks pending interrupts and execute clearers and handlers if
   one is registered.
 */
void irq_handler(void) {
  unsigned int j; 
  for (j = 0; j < NUM_IRQS; j++) {
    // If the interrupt is pending and there is a handler, run the handler
    if (IRQ_IS_PENDING(interrupt_regs, j) && (handlers[j] != &do_nothing)) {
      clearers[j]();
      ENABLE_INTERRUPTS(); // We might want to perform a syscall or something in the handler...
      handlers[j]();
      DISABLE_INTERRUPTS();
      return;
    }
  }
}

/* This function is going to be called by the processor on each
   syscall (see interrupts_vector.S to see why).
   It simply calls the appropriate function to treat the syscall.
 */
void software_interrupt_handler(unsigned int r0, unsigned int r1 ) {
  DISABLE_INTERRUPTS();

  if(r0 < syscalls_table_length)
    syscalls_table[r0] ((void*)r1);
  else
    {} // TODO: add error treatment here

   ENABLE_INTERRUPTS();
}

void register_irq_handler(irq_number_t irq_num, interrupt_handler_f handler,
			  interrupt_clearer_f clearer) {
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
    puts("Error: cannot register IRQ handler: invalid IRQ number: \n");
  }
}
void unregister_irq_handler(irq_number_t irq_num) {
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
    puts("Error: cannot unregister IRQ handler: invalid IRQ number: \n");
  }
}

/* The following functions are unimplemented and should never be called. */

void __attribute__ ((interrupt ("ABORT"))) reset_handler(void) {
  puts("RESET HANDLER\n");
  while(1){}
}
void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void) {
  puts("UNDEFINED INSTRUCTION HANDLER\n");
  while(1){}
}
void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void) {
  while(1)
    puts("PREFETCH ABORT HANDLER\n");
}
void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void) {
  while(1)
    puts("DATA ABORT HANDLER\n");
}
void __attribute__ ((interrupt ("FIQ"))) fast_irq_handler(void) {
  while(1)
    puts("FIQ HANDLER\n");
}
