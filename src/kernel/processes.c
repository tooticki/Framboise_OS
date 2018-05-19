#include "../common/stdlib.h"

#include "timer.h"
#include "interrupts.h"
#include "mem.h"
#include "priority_scheduler.h"
#include "processes.h"

// Processes

static uint32_t next_proc_num = 1;
#define NEW_PID next_proc_num++;

extern uint8_t __end; // From boot.S
extern void switch_to_thread(process_control_block_t * old, process_control_block_t * new); // From context_switch.S

void processes_init(void) {
  process_control_block_t * main_pcb;
 
  // Allocate and initailize the block
  main_pcb = kmalloc(sizeof(process_control_block_t));
  main_pcb->stack_page = (void *)&__end;
  main_pcb->pid = NEW_PID;
  memcpy(main_pcb->process_name, "Init", 5);

  scheduler_init(main_pcb);
  
  // Set the timer to go off after 10 ms
  timer_set(10000);
}

process_control_block_t * create_process(kthread_function_f thread_func, char * name, int name_len) {
  process_control_block_t * pcb;
  process_saved_state_t * new_proc_state;

  // Allocate and initialize the pcb
  pcb = kmalloc(sizeof(process_control_block_t));
  pcb->stack_page = alloc_page();
  pcb->pid = NEW_PID;
  memcpy(pcb->process_name, name, MIN(name_len,18));
  pcb->process_name[MIN(name_len,19)] = 0;

  // Get the location the of the stack pointer at the moment
  new_proc_state = pcb->stack_page + PAGE_SIZE - sizeof(process_saved_state_t);
  pcb->saved_state = new_proc_state;

  // Set up the stack that will be restored during a context switch
  bzero(new_proc_state, sizeof(process_saved_state_t));
  new_proc_state->lr = (uint32_t)thread_func; // lr is used as return address in switch_to_thread
  new_proc_state->sp = (uint32_t)reap;        // When the thread function returns, this reaper routine will clean it up
  new_proc_state->cpsr = 0x13 | (8 << 1);     // Sets the thread up to run in supervisor mode with irqs only
  return pcb;
}

void schedule(void) {
  DISABLE_INTERRUPTS();
  process_control_block_t * old_thread = get_current_process(), * new_thread = pop_process(); //The order should be kept since pop_process changes the current process
  
  // If there are no more processes to run then continue running the current one
  if(new_thread == 0){ 
    timer_set(10000);
    ENABLE_INTERRUPTS();
    return;
  }
   push_current_process(); // Push the old process in the run queue
  // Context Switch
  // Implemented in context_switch.S. This never returns, calls
  // set_timer(10000), enables interrupts
  switch_to_thread(old_thread, new_thread); 
}

void reap(void){ // Free all resources associated with a process, context switch immediately
  DISABLE_INTERRUPTS();
  process_control_block_t * old_thread = get_current_process(),  * new_thread = pop_another_process(); //The order should be kept since pop_process changes the current process
  // Do nothing while all run queues are empty
  while(new_thread == 0){ 
      new_thread = pop_another_process();
  }

  // Free the resources used by the old process. Technically, we are
  // using dangling pointers here, but since interrupts are disabled
  // and we only have one core, it should still be fine
  free_page(old_thread->stack_page);
  kfree(old_thread);

  // Context Switch: This never returns
  // calls set_timer(10000), enables interrupts
  switch_to_thread(old_thread, new_thread);
}

int kill(uint32_t pid){
  if (pid == current_pid()){
    reap();
    return 1;
  }
  DISABLE_INTERRUPTS();
  int res = remove_by_pid(pid);
  ENABLE_INTERRUPTS();
  return res;
}

