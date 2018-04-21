#include "../common/stdlib.h"
#include "processes.h"
#include "timer.h"
#include "uart.h"
#include "interrupts.h"
#include "mem.h"

// PCB (Process Control Block) List
void push_pcb(pcb_list_t * l, process_control_block_t * p){ // Adds a pcb in the end of the list l
  if(l->first == 0){
    l->first = p;
    l->last = p;
  }
  else{
    l->last->next_pcb = p;
    l->last = p;
  }
}

process_control_block_t * pop_pcb(pcb_list_t * l){ // Pops a pcb from the beginning of the list l
  if(l->first == 0)
    return 0;
  else{
    process_control_block_t * tmp = l->first;
    l->first = l->first->next_pcb;
    if(l->first == 0)
      l->last = 0;
    return tmp;
  }
}

process_control_block_t * peek_pcb(pcb_list_t * l){ // Peeks a pcb from the beginning of the list l
  return l->first;
}

// Processes

static uint32_t next_proc_num = 1;
#define NEW_PID next_proc_num++;
process_control_block_t * current_process;
pcb_list_t run_queue;      // Run Queue = list of processes willing to run
pcb_list_t all_processes;  // List of all processes

extern uint8_t __end; // From boot
extern void switch_to_thread(process_control_block_t * old, process_control_block_t * new); // From boot

void processes_init(void) {
  // run_queue = kmalloc(sizeof(pcb_list_t));
  // all_peocesses = kmalloc(sizeof(pcb_list_t));
  process_control_block_t * main_pcb;
  
  // Allocate and initailize the block
  main_pcb = kmalloc(sizeof(process_control_block_t));
  main_pcb->stack_page = (void *)&__end;
  main_pcb->pid = NEW_PID;
  memcpy(main_pcb->process_name, "Init", 5);

  // Add main_pcb to all process list.  It is already running, so don't add it to the run queue
  push_pcb(&all_processes, main_pcb);
  main_pcb->next_pcb = 0;

  current_process = main_pcb;

  // Set the timer to go off after 10 ms
  timer_set(10000);
}


void schedule(void) {
  uart_puts("\nWe're scheduling\n");
  // DISABLE_INTERRUPTS();
  process_control_block_t * new_thread, * old_thread;
#if 0
  // If the run queue is empty, the current process continues
  if (run_queue.first == 0)
    return;

  // Get the next thread to run.  For now we are using round-robin (FIFO)
  new_thread = pop_pcb(&run_queue);
  old_thread = current_process;
  current_process = new_thread;

  // Put the current thread back in the run queue
  push_pcb(&run_queue, old_thread);

  // Context Switch 
  switch_to_thread(old_thread, new_thread); // in context_switch.S
#endif
  // ENABLE_INTERRUPTS();
  uart_puts("\nScheduling is done\n");
}

void create_process(kthread_function_f thread_func, char * name, int name_len) {
  process_control_block_t * pcb;
  process_saved_state_t * new_proc_state;

  // Allocate and initialize the pcb
  pcb = kmalloc(sizeof(process_control_block_t));
  pcb->stack_page = alloc_page();
  pcb->pid = NEW_PID;
  memcpy(pcb->process_name, name, MIN(name_len,19));
  pcb->process_name[MIN(name_len,19)+1] = 0;

  // Get the location the stack pointer should be in when this is run
  new_proc_state = pcb->stack_page + PAGE_SIZE - sizeof(process_saved_state_t);
  pcb->saved_state = new_proc_state;

  // Set up the stack that will be restored during a context switch
  bzero(new_proc_state, sizeof(process_saved_state_t));
  new_proc_state->lr = (uint32_t)thread_func;     // lr is used as return address in switch_to_thread
  new_proc_state->sp = (uint32_t)reap;            // When the thread function returns, this reaper routine will clean it up
  new_proc_state->cpsr = 0x13 | (8 << 1);         // Sets the thread up to run in supervisor mode with irqs only

  // add the thread to the lists
  push_pcb(&all_processes, pcb);
  push_pcb(&run_queue, pcb);
}


static void reap(void){ // Free all resources associated with a process, context switch immediately
  // DISABLE_INTERRUPTS();
  process_control_block_t * new_thread, * old_thread;

  // Do nothing wkile the run queue is empty
  while (run_queue.first == 0);

  // Get the next thread to run. For now we are using round-robin
  new_thread = pop_pcb(&run_queue);
  old_thread = current_process;
  current_process = new_thread;

  // Free the resources used by the old process.  Technically, we are using dangling pointers here, but since interrupts are disabled and we only have one core, it
  // should still be fine
  free_page(old_thread->stack_page);
  kfree(old_thread);

  // Context Switch
  switch_to_thread(old_thread, new_thread);

  // TODO: enable interrupts?
}
