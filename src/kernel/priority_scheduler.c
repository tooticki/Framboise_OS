/* Priority-Based Round Robin Scheduling Algorithm

   Round Robin schedule algorithm works as following: all processes
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

   The scheduling algorithm used here is the last one.
   Processes are wrapped in "nodes"
*/
#include "../common/stdlib.h"
#include "../common/stdio.h"

#include "uart.h"
#include "mem.h"
#include "processes.h"
#include "priority_scheduler.h"

/**************************************************** Run Queue ****************************************************/
// Node which wraps a process control block (PCB)
typedef struct node{
  process_control_block_t * process; // PCB itself
  uint32_t priority;             // Priority of this process
  struct node * next_node;           // The next PCB-node in its Run Queue  
} node_t;

// Run Queue is the list of processes willing to run
typedef struct {
  node_t * first;
  node_t * last;
} run_queue_t;

void push_node(run_queue_t * l, node_t * p);       // Add a node in the end of the list l
node_t * pop_node(run_queue_t * l);                // Pop a node from the beginning of the list l
node_t * peek_node(run_queue_t * l);               // Peek a node from the beginning of the list l
node_t * find_node(run_queue_t * l, uint32_t pid); // Look for the process with a given pid, return 0 if not found
void remove_node(run_queue_t * l, node_t * p);     // Remove a pcb-node from the list l (only if p was in l)

// Run Queue functions
void push_node(run_queue_t * q, node_t * p){
  if(q->first == 0){
    q->first = p;
    q->last = p;
  }
  else{
    q->last->next_node = p;
    q->last = p;
  }
  p->next_node = 0;
}

node_t * pop_node(run_queue_t * q){
  if(q->first == 0)
    return 0;
  else{
    node_t * tmp = q->first;
    q->first = q->first->next_node;
    if(q->first == 0)
      q->last = 0;
    return tmp;
  }
}

node_t * peek_node(run_queue_t * q){
  return q->first;
}

node_t * find_node(run_queue_t * q, uint32_t pid){
  node_t * tmp = q->first;
  while(tmp != 0 && tmp->process->pid != pid)
    tmp = tmp->next_node;
  return tmp;
}

void remove_node(run_queue_t * q, node_t * p){
  if(p == 0) return;
  node_t * tmp = q->first;
  if(tmp == 0) return;
  if(q->first == p){
    pop_node(q);
    return;
  }
  while(tmp->next_node != p && tmp->next_node != 0){
    tmp = tmp->next_node;
  }  
  if(tmp == 0) return;  
  tmp->next_node = p->next_node;
  p->next_node = 0;
}

/**************************************************** Priority-Based Round Robin Scheduler ****************************************************/

/* Priorities are numbers from 0 to MAX_PRIORITY
 The greater the priority of the process, the more CPU time
 it is supposed to take.
*/
#define MAX_PRIORITY 10

node_t * current_node;     // Currently running process
node_t * old_current_node; // The previous process

// Here, run_queue[p] is a queue of processes with priority p
run_queue_t run_queue[MAX_PRIORITY+1];     

process_control_block_t * get_current_process(void){
  return current_node->process;
}

uint32_t current_pid(){
  return current_node->process->pid;
}

void scheduler_init(process_control_block_t * init_process){ //should be called inside processes_init()
  node_t * init_node = kmalloc(sizeof(node_t));
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

void register_process(process_control_block_t * process, void * arg){
  uint32_t priority = *((uint32_t *) arg);
  node_t * new_node = kmalloc(sizeof(node_t));
  new_node->process = process;
  new_node->priority = priority;
  new_node->next_node = 0;
  push_node(&run_queue[priority], new_node); // Add process to the queue of the needed priority
}


process_control_block_t * pop_process(){
  int p = MAX_PRIORITY, old_p = current_node->priority;
  node_t * new_node;
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
  node_t * new_node;
  while(p >= 0 && run_queue[p].first == 0) p--; // Find a nonempty queue of the highest priority
  
  // If there are no processes, nothing happens
  if (p < 0){
    return 0;
  }  
  // Get the next thread to run
  new_node = pop_node(&run_queue[p]);
  kfree(current_node); // will be never called anymore
  old_current_node = 0;
  current_node = new_node;
  return new_node->process;
}

void push_current_process(){
  // Put the current thread back in the run queue
  push_node(&run_queue[old_current_node->priority], old_current_node);
}

int remove_by_pid(uint32_t pid){
  node_t * found_node = 0;
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

/**************************************************** Functions Displaying Information ****************************************************/

// Print for each queue if it's empty or not
void run_queues_report(void) {
  for(int i = 0; i <= MAX_PRIORITY; i++){
    puts(itoa(i));
    if(run_queue[i].first == 0) 
      puts("th run queue is empty\n");
    else 
      puts("th run queue has something inside\n");
  }
}

 // Display the name, pid and priority of the current process
void process_report(node_t * p){
  puts("Process ");
  puts(p->process->process_name);
  puts(", pid ");
  puts(itoa(p->process->pid));
  puts(", priority ");
  puts(itoa(p->priority));
  puts(" ");
}

// Display the name, pid and priority of the current process
void current_process_report(){
  process_report(current_node);
}

// Display the list of running processes with their names, pids and priorities
void print_processes_list(void){
  node_t * p;
  process_report(current_node);
  puts("  (running) \n");
  for(int i = 0; i <= MAX_PRIORITY; i++){
    p = run_queue[i].first;
    while(p != 0){
      process_report(p);
      puts("\n");
      p = p->next_node;
    }
  }  
}

/**************************************************** Fill in the Priority Scheduler ****************************************************/

scheduler_t priority_scheduler = {
  scheduler_init,
  register_process,
  pop_process,
  pop_another_process,
  push_current_process,
  remove_by_pid,
  
  get_current_process,
  current_pid,

  current_process_report,
  print_processes_list
};

