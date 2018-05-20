#include <limits.h>

#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "mem.h"
#include "rm_scheduler.h"

// PCB-List functions
void push_node_rm(run_queue_t * q, node_rm_t * p){
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

node_rm_t * pop_node_rm(run_queue_t * q){ //A queue is never empty
  node_rm_t * tmp = q->first;
  q->first = q->first->next_node;
  if(q->first == 0){
    remove_queue(q);
  }
  return tmp;
}

node_rm_t * peek_node_rm(run_queue_t * q){
  return q->first;
}

node_rm_t * find_node_rm(run_queue_t * q, uint32_t pid){
  node_rm_t * tmp = q->first;
  while(tmp != 0 && tmp->process->pid != pid)
    tmp = tmp->next_node;
  return tmp;
}

void remove_node_rm(run_queue_t * q, node_rm_t * p){ //A queue is never empty
  if(p == 0) return;
  node_rm_t * tmp = q->first;
  if(q->first == p){
    pop_node_rm(q);
    return;
  }
  while(tmp->next_node != p && tmp->next_node != 0){
    tmp = tmp->next_node;
  }  
  if(tmp == 0) return;  
  tmp->next_node = p->next_node;
  p->next_node = 0;
}

// Queue list

/* We have a run queue for each period, they are stored in a chained
   list, ordered by periods; no queue can be empty;
   here is the first queue: */
run_queue_t * first_queue; // The queue with the shortest period

// Queue list functions

void new_queue(run_queue_t * q, node_rm_t * p){
  q = kmalloc(sizeof(run_queue_t));
  q->period = p->period;
  q->first = p;
  q->last = p;
  q->next_queue = 0;
}

void add_to_queues(node_rm_t * p){ // p is supposed to be nonzero
  if(first_queue == 0){
    new_queue(first_queue, p);
    return;
  }
  run_queue_t * q = first_queue, * tmp = 0;
  if(first_queue->period == p->period){
    push_node_rm(first_queue, p);
    return;
  }
  if(first_queue->period > p->period){
    new_queue(q, p);
    q->next_queue = first_queue->next_queue;
    first_queue = q;
    return;
  }
  q = first_queue;
  while(q->next_queue != 0 && q->next_queue->period < p->period)
    q = q->next_queue;
  if(q->next_queue == 0){ // We reached the end
    new_queue(q->next_queue, p);
    return;
  }
  if(q->next_queue->period == p->period){
    push_node_rm(q->next_queue, p);
    return;
  }
  // q->next->period > p->period > q->period
  new_queue(tmp, p);
  tmp->next_queue = q->next_queue;
  q->next_queue = tmp;
}

void remove_queue(run_queue_t * q){ // Called only if q is among queues
  if(first_queue == q){
    first_queue = first_queue->next_queue;
    return;
  }
  run_queue_t * tmp = first_queue;
  while(tmp->next_queue != 0 && tmp->next_queue != q)
    tmp = tmp->next_queue;
  if(tmp == 0)
    return;
  tmp->next_queue = tmp->next_queue->next_queue;
  kfree(q);
}
// Scheduler

node_rm_t * current_node;     // Currently running process
node_rm_t * old_current_node; // The previous process   

process_control_block_t * rm_get_current_process(void){
  return current_node->process;
}

uint32_t rm_current_pid(){
  return current_node->process->pid;
}

void rm_scheduler_init(process_control_block_t * init_process){ //should be called inside processes_init()
  node_rm_t * init_node = kmalloc(sizeof(node_rm_t));
  // No run queues for now
  first_queue = 0;
  
  // Define the current node.  It is already running, so don't add it to the run queues
  init_node->process = init_process;
  init_node->runtime = 0;
  init_node->period = UINT_MAX; // By default, the running process' priority is 0, thus the deadline is far
  init_node->next_node = 0;
  current_node = init_node;
  old_current_node = 0;
}

// Stores information about a process
// Used as an argument for register_process(process, arg)
typedef struct{
  uint32_t period;
  uint32_t runtime;
} info_process_t;

void rm_register_process(process_control_block_t * process, void * arg){
  info_process_t * info = (info_process_t *) arg;
  node_rm_t * new_node = kmalloc(sizeof(node_rm_t));
  new_node->process = process;
  new_node->runtime = info->runtime;
  new_node->period = info->period;
  new_node->next_node = 0;
  add_to_queues(new_node); // Add process to the queues
}

process_control_block_t * rm_pop_process(){
  uint32_t current_period = current_node->period;
  node_rm_t * new_node;
  // If there are no processes with shorter deadline,
  // the current process continues to run
  if(first_queue == 0 || first_queue->period > current_period){
    return 0;
  }
  // Get the next thread to run
  new_node = pop_node_rm(first_queue);
  old_current_node = current_node;
  current_node = new_node;
  return new_node->process;
}

process_control_block_t * rm_pop_another_process(){ // Should free the node of the current_process
  node_rm_t * new_node;
  // If there are no processes, nothing happens
  if(first_queue == 0){
    return 0;
  }
  // Get the next thread to run
  new_node = pop_node_rm(first_queue);
  kfree(current_node); // will be never called anymore
  old_current_node = 0;
  current_node = new_node;
  return new_node->process;
}

void rm_push_current_process(){
  //Put the old current thread back in the run queues
  add_to_queues(old_current_node);
}

int rm_remove_by_pid(uint32_t pid){
  run_queue_t * q = first_queue;
  node_rm_t * found_node = 0;
  while(q != 0){
    found_node = find_node_rm(q, pid);
    if(found_node != 0){
      remove_node_rm(q, found_node);
      break;
    }
    q = q->next_queue;
  }
  if(found_node == 0)
    return -1;
  return 1;
}

// Print information functions

void rm_run_queues_report(void) {
  run_queue_t * tmp = first_queue;
  if(tmp == 0){
    puts("No run queues :-(\n");
  }
  while(tmp != 0){
    puts("Run queue of period ");
    puts(itoa(tmp->period));
    puts("\n");
    tmp = tmp -> next_queue;
  }
}

void rm_process_report(node_rm_t * p){
  puts("Process ");
  puts(p->process->process_name);
  puts(", pid ");
  puts(itoa(p->process->pid));
  puts(", runtime ");
  puts(itoa(p->runtime));
  puts(", period ");
  puts(itoa(p->period));
}

void rm_current_process_report(){
  rm_process_report(current_node);
}

void rm_print_processes_list(void){
  run_queue_t * q = first_queue;
  node_rm_t * p;
  rm_current_process_report();
  puts("  (running) \n");
  while(q != 0){
    p = q->first;
    while(p != 0){
      rm_process_report(p);
      puts("\n");
      p = p->next_node;
    }
    q = q->next_queue;
  }  
}



