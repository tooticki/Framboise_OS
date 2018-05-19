#include "../common/stdlib.h"
#include "../common/stdio.h"

#include "uart.h"
#include "mem.h"
#include "processes.h"
#include "priority_scheduler.h"


// PCB (Process Control Block) List
void push_node(pcb_list_t * l, pcb_node_t * p){ // Adds a pcb in the end of the list l
  if(l->first == 0){
    l->first = p;
    l->last = p;
  }
  else{
    l->last->next_node = p;
    l->last = p;
  }
  p->next_node = 0;
}

pcb_node_t * pop_node(pcb_list_t * l){ // Pops a pcb from the beginning of the list l
  if(l->first == 0)
    return 0;
  else{
    pcb_node_t * tmp = l->first;
    l->first = l->first->next_node;
    if(l->first == 0)
      l->last = 0;
    return tmp;
  }
}

pcb_node_t * peek_node(pcb_list_t * l){ // Peeks a pcb from the beginning of the list l
  return l->first;
}

pcb_node_t * find_node(pcb_list_t * l, uint32_t pid){ // Finds the pcb with a given pid, returns 0 if not found
  pcb_node_t * tmp = l->first;
  while(tmp != 0 && tmp->process->pid != pid)
    tmp = tmp->next_node;
  return tmp;
}

void remove_node(pcb_list_t * l, pcb_node_t * p){ // Removes a pcb from the list l (only if p was in l)
  if(p == 0) return;
  pcb_node_t * tmp = l->first;
  if(tmp == 0) return;
  if(l->first == p){
    pop_node(l);
    return;
  }
  while(tmp->next_node != p && tmp->next_node != 0){
    tmp = tmp->next_node;
  }  
  if(tmp == 0) return;  
  tmp->next_node = p->next_node;
  p->next_node = 0;
}

// Scheduler

// Priorities are numbers from 0 to MAX_PRIORITY
// The greater the priority of the process, the more CPU time it is
// supposed to take.
#define MAX_PRIORITY 10
pcb_node_t * current_node;
pcb_node_t * old_current_node;

// Run Queue = list of processes willing to run.
// run_queue[p] is a queue of processes with priority p.
pcb_list_t run_queue[MAX_PRIORITY+1];     

process_control_block_t * get_current_process(void){
  return current_node->process;
}

uint32_t current_pid(){
  return current_node->process->pid;
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
  current_node = init_node;
}

void register_process(process_control_block_t * process, unsigned int priority){
  pcb_node_t * new_node = kmalloc(sizeof(pcb_node_t));
  new_node->process = process;
  new_node->priority = priority;
  new_node->next_node = 0;
  push_node(&run_queue[priority], new_node); // Add process to the queue of the needed priority
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
process_control_block_t * pop_process(){
  int p = MAX_PRIORITY, old_p = current_node->priority;
  pcb_node_t * new_node;
  while(p >= old_p && run_queue[p].first == 0) p--; // Find a nonempty queue of the highest priority
  
  // If there are no processes with higher priority, the current
  // process continues to run
  if (p < old_p){
    return 0;
  }  
  // Get the next thread to run
  new_node = pop_node(&run_queue[p]);
  old_current_node = current_node;
  current_node = new_node;
  return new_node->process;
}

process_control_block_t * pop_another_process(){
  int p = MAX_PRIORITY;
  pcb_node_t * new_node;
  while(p >= 0 && run_queue[p].first == 0) p--; // Find a nonempty queue of the highest priority
  
  // If there are no processes, nothing happens
  if (p < 0){
    return 0;
  }  
  // Get the next thread to run
  new_node = pop_node(&run_queue[p]);
  old_current_node = current_node;
  current_node = new_node;
  return new_node->process;
}

void push_current_process(){
  // Put the current thread back in the run queue
  push_node(&run_queue[old_current_node->priority], old_current_node);
}

int remove_by_pid(uint32_t pid){
  pcb_node_t * found_node = 0;
  for(int i = 0; i <= MAX_PRIORITY; i++){
    found_node = find_node(&run_queue[i], pid);
    if(found_node != 0){
      remove_node(&run_queue[i], found_node);
      break;
    }
  }
  if(found_node == 0)
    return -1;
  return 1;
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
  pcb_node_t * tmp;
  puts("name: ");
  puts(current_node->process->process_name);
  puts(" pid:");
  puts(itoa(current_node->process->pid));
  puts(" priority:");
  puts(itoa(current_node->priority));
  puts("  (running) \n");
  for(int i = 0; i <= MAX_PRIORITY; i++){
    tmp = run_queue[i].first;
    while(tmp != 0){
      puts("name: ");
      puts(tmp->process->process_name);
      puts(" pid:");
      puts(itoa(tmp->process->pid));
      puts(" priority:");
      puts(itoa(i));
      puts("\n");
      tmp = tmp->next_node;
    }
  }
}

void process_report(void){
  pcb_node_t * node = current_node;
  puts("I'm process ");
  puts(node->process->process_name);
  puts(", pid ");
  puts(itoa(node->process->pid));
  puts(", priority ");
  puts(itoa(node->priority));
  puts(": ");
}
