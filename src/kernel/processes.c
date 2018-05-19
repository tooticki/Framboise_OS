#include "../common/stdlib.h"
#include "../common/stdio.h"

#include "timer.h"
#include "uart.h"
#include "interrupts.h"
#include "mem.h"
#include "processes.h"

// PCB (Process Control Block) List
void push_node(pcb_list_t * l, pcb_node_t * p){ // Adds a pcb in the end of the list l
  if(l->first == 0){
    l->first = p;
    l->last = p;
  }
  else{
    l->last->next_pcb = p;
    l->last = p;
  }
  p->next_pcb = 0;
}

process_control_block_t * pop_node(pcb_list_t * l){ // Pops a pcb from the beginning of the list l
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

process_control_block_t * peek_node(pcb_list_t * l){ // Peeks a pcb from the beginning of the list l
  return l->first;
}

process_control_block_t * find_node(pcb_list_t * l, uint32_t pid){ // Finds the pcb with a given pid, returns 0 if not found
  pcb_node_t * tmp = l->first;
  while(tmp != 0 && tmp->process->pid != pid)
    tmp = tmp->next_node;
  return tmp;
}

void remove_node(pcb_list_t * l, process_control_block_t * p){ // Removes a pcb from the list l (only if p was in l)
  if(p == 0) return;
  pcb_node_t * tmp = l->first;
  if(tmp == 0) return;
  if(l->first == p){
    pop_pcb(l);
    return;
  }
  while(tmp->next_pcb != p && tmp->next_pcb != 0){
    tmp = tmp->next_pcb;
  }  
  if(tmp == 0) return;  
  tmp->next_pcb = p->next_pcb;
  p->next_pcb = 0;
}

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

/* Round Robin schedule algorithm works as following: all processes
   are stored in the run queue and each time quantum, the next process
   in the queue replaces the process working currently.

   Priority-based Round Robin is based on the previous and features
   handling of priorities: for each priority value, we store a run
   queue. While scheduling, we first check the processes with the
   highest proprity MAX_PRIORITY, and descend to queues with lower
   priorities only if it was empty. We repeat this until a process is
   found or the current priority is reached. If there is no processes
   in the current priority queue, continue running the current
   process.

   The algorithm we use here is the last one.
*/

void schedule(void) {
  DISABLE_INTERRUPTS();
  process_control_block_t * new_thread, * old_thread = get_current_process();
  new_thread = pop_process();
  // If there are no more processes to run then continue running the current one
  if(new_thread = 0){ 
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
  process_control_block_t * new_thread = 0;
  // Do nothing while all run queues are empty
  while(new_thread = 0){ 
      new_thread = pop_process();
  }


  
  // Get the next thread to run. For now we are using round-robin
  new_thread = pop_pcb(&run_queue[p]);
  old_thread = current_process;
  current_process = new_thread;

  // Update the current priority
  current_priority = p;
  
  // Free the resources used by the old process. Technically, we are
  // using dangling pointers here, but since interrupts are disabled
  // and we only have one core, it should still be fine
  free_page(old_thread->stack_page);
  kfree(old_thread);

  // Context Switch: enable interrupts, sets timer, never returns
  switch_to_thread(old_thread, new_thread);
}

int kill(uint32_t pid){
  process_control_block_t * found_process = 0;
  if (pid == current_process->pid){
    reap();
    return 1;
  }
  for(int i = 0; i <= MAX_PRIORITY; i++){
    found_process = find_pcb(&run_queue[i], pid);
    if(found_process != 0){
      remove_pcb(&run_queue[i], found_process);
      break;
    }
  }
  if(found_process == 0)
    return -1;
  return 1;
}

// Scheduler

// Priorities are numbers from 0 to MAX_PRIORITY
// The greater the priority of the process, the more CPU time it is
// supposed to take.
#define MAX_PRIORITY 10
pcb_node_t * current_node;

// Run Queue = list of processes willing to run.
// run_queue[p] is a queue of processes with priority p.
pcb_list_t run_queue[MAX_PRIORITY+1];     

process_control_block_t * get_current_process(void){
  return current_node->process;
}

void scheduler_init(process_control_block_t * init_process){ //should be called inside processes_init()
  pcb_node_t * init_node = kmalloc(sizeof(pcb_node_t));
  for(int i = 0; i <= MAX_PRIORITY; i++){
    run_queue[i].first = 0;
    run_queue[i].last = 0;
  }
  // Define the current node.  It is already running, so don't add it to the run queues
  init_node->process = init_process;
  init_node->priority = 0; // By default, the running process' priority is 0
  init_node->next_node = 0;
  current_node = tmp;
}

void register_process(process_control_block_t * process, unsigned int priority){
  pcb_node_t * new_node = kmalloc(sizeof(pcb_node_t));
  new_node->process = process;
  new_node->priority = priority;
  new_node->next_pcb = 0;
  push_pcb(&run_queue[priority], new_node); // Add process to the queue of the needed priority
}

process_control_block_t * pop_process(){
  int p = MAX_PRIORITY, old_p = current_node->priority;
  pcb_node_t  new_node;
  while(p >= old_p && run_queue[p].first == 0) p--; // Find a nonempty queue of the highest priority
  
  // If there are no processes with higher priority, the current
  // process continues to run
  if (p < old_p){
    return 0;
  }
  
  // Get the next thread to run
  new_node = pop_node(&run_queue[p]);
  current_node = new_node;
  return new_node->process;
}

void push_current_process(){
  // Put the current thread back in the run queue
  push_node(&run_queue[current_node->priority], current_node);
}

// Print information functions

void run_queues_report(void) {
  for(int i = 0; i <= MAX_PRIORITY; i++){
    puts(itoa(i));
    if(run_queue[i].first == 0) 
      puts("th run queue is empty\n");
    else 
      puts("th run queue has something inside\n");
  }
}

void print_processes_list(void){
  process_control_block_t * tmp;
  for(int i = 0; i <= MAX_PRIORITY; i++){
    tmp = run_queue[i].first;
    while(tmp != 0){
      puts("name: ");
      puts(tmp->process_name);
      puts(" pid:");
      puts(itoa(tmp->pid));
      puts(" priority:");
      puts(itoa(i));
      puts("\n");
      tmp = tmp->next_pcb;
    }
  }
}

void process_report(void){
  process_control_block_t pcb = *current_process;
  puts("\nI'm process ");
  puts(pcb.process_name);
  puts(", pid ");
  puts(itoa(pcb.pid));
  puts(": ");
}
