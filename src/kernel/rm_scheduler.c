/* Rate Monotonic Scheduling Algorithm

   For this variant of the Rate Monotonic scheduling algorithm,
   several assumptions should hold:
     
     1. Each process is periodic. That means it's requested each p
     microseconds and it should be completed befoer the next request;
     p is called a period of a process, it's fixed

     2. For each process, its runtime is constant and is known when the
     process is created. Runtime refers to the time which is taken by
     a processor to execute the process without interruption

   The scheduling algorithm works like this:

   Each Run Queue has its period; Run Queues are stored in the list.
   Each newly created process is added to the Run Queue according to
   its period.
   Each time scheduling happens, the head process from the queue of
   the shortest period start to run; the previus running process goes
   to the tail of the queue.
*/
#include <limits.h>

#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "mem.h"
#include "rm_scheduler.h"

/**************************************************** Run Queue ****************************************************/
// Node which wraps a process control block (PCB)
typedef struct node{
  process_control_block_t * process; // PCB itself
  uint32_t runtime;                  // Run time of this process, in microiseconds
  uint32_t period;                   // Period of this process, in microiseconds
  struct node * next_node;           // The next node in its Run Queue  
} node_t;

// Run Queue
typedef struct run_queue{
  uint32_t period;               // Period of processes in this queue
  node_t * first;                // First node
  node_t * last;                 // Last node
  struct run_queue * next_queue; // The next queue in the list
} run_queue_t;

// Run Queue functions
static void push_node(run_queue_t * q, node_t * p);       // Add a node p in the end of the queue q
static node_t * pop_node(run_queue_t * q);                // Pop a node from the beginning of the queue q
static node_t * peek_node(run_queue_t * q);               // Peek a node from the beginning of the queue q
static node_t * find_node(run_queue_t * q, uint32_t pid); // Look for the process with a given pid, return 0 if not found
static void remove_node(run_queue_t * q, node_t * p);     // Remove a node p from the queue q (only if p was in q)

// Run Queue List functions
void new_queue(run_queue_t * q, node_t * p); // Allocate memory for q, make p its single element
void add_to_queues(node_t * p);              // Put p into a queue with the appropriate deadline
void remove_queue(run_queue_t * q);             // Remove q from the list of queues

static void push_node(run_queue_t * q, node_t * p){
  p->next_node = 0;
  if(q->first == 0){
    q->first = p;
    q->last = p;
  }
  else{
    q->last->next_node = p;
    q->last = p;
  }
}

static node_t * pop_node(run_queue_t * q){ //A queue is never empty
  node_t * tmp = q->first;
  q->first = q->first->next_node;
  if(q->first == 0){
    remove_queue(q);
  }
  return tmp;
}

static node_t * peek_node(run_queue_t * q){
  return q->first;
}

static node_t * find_node(run_queue_t * q, uint32_t pid){
  node_t * tmp = q->first;
  while(tmp != 0 && tmp->process->pid != pid)
    tmp = tmp->next_node;
  return tmp;
}

static void remove_node(run_queue_t * q, node_t * p){ //A queue is never empty
  if(p == 0) return;
  node_t * tmp = q->first;
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

/**************************************************** List of Run Queues ****************************************************/

/* There is one Run Queue for each period. Run Queues are stored in a
   chained list, ordered by periods; no queue is empty in this list;
   here is the pointer to the first queue:
*/
run_queue_t * first_queue; // The queue with the shortest period

void new_queue(run_queue_t * q, node_t * p){
  q = kmalloc(sizeof(run_queue_t));
  q->period = p->period;
  q->first = p;
  q->last = p;
  q->next_queue = 0;
}

void add_to_queues(node_t * p){ // p is supposed to be nonzero
  if(first_queue == 0){
    new_queue(first_queue, p);
    return;
  }
  run_queue_t * q = first_queue, * tmp = 0;
  if(first_queue->period == p->period){
    push_node(first_queue, p);
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
    push_node(q->next_queue, p);
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

/**************************************************** Rate Monotonic Scheduler ****************************************************/

node_t * current_node;     // Currently running process
node_t * old_current_node; // The previous process   

static process_control_block_t * get_current_process(void){
  return current_node->process;
}

static uint32_t current_pid(){
  return current_node->process->pid;
}

static void scheduler_init(process_control_block_t * init_process){ //should be called inside processes_init()
  node_t * init_node = kmalloc(sizeof(node_t));
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

// info_process_t stores information about a process
// Used as an argument for register_process(process, arg)
typedef struct{
  uint32_t period;
  uint32_t runtime;
} info_process_t;

static void register_process(process_control_block_t * process, void * arg){
  info_process_t * info = (info_process_t *) arg;
  node_t * new_node = kmalloc(sizeof(node_t));
  new_node->process = process;
  new_node->runtime = info->runtime;
  new_node->period = info->period;
  new_node->next_node = 0;
  add_to_queues(new_node); // Add process to the queues
}

static process_control_block_t * pop_process(){
  uint32_t current_period = current_node->period;
  node_t * new_node;
  // If there are no processes with shorter deadline,
  // the current process continues to run
  if(first_queue == 0 || first_queue->period > current_period){
    return 0;
  }
  // Get the next thread to run
  new_node = pop_node(first_queue);
  old_current_node = current_node;
  current_node = new_node;
  return new_node->process;
}

static process_control_block_t * pop_another_process(){ // Should free the node of the current_process
  node_t * new_node;
  // If there are no processes, nothing happens
  if(first_queue == 0){
    return 0;
  }
  // Get the next thread to run
  new_node = pop_node(first_queue);
  kfree(current_node); // will be never called anymore
  old_current_node = 0;
  current_node = new_node;
  return new_node->process;
}

static void push_current_process(){
  //Put the old current thread back in the run queues
  add_to_queues(old_current_node);
}

static int remove_by_pid(uint32_t pid){
  run_queue_t * q = first_queue;
  node_t * found_node = 0;
  while(q != 0){
    found_node = find_node(q, pid);
    if(found_node != 0){
      remove_node(q, found_node);
      break;
    }
    q = q->next_queue;
  }
  if(found_node == 0)
    return -1;
  return 1;
}

/**************************************************** Functions Displaying Information ****************************************************/

 // Print for each queue its period
static void run_queues_report(void) {
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

// Display the name, pid, runtime and period of the process
static void process_report(node_t * p){ 
  puts("Process ");
  puts(p->process->process_name);
  puts(", pid ");
  puts(itoa(p->process->pid));
  puts(", runtime ");
  puts(itoa(p->runtime));
  puts(", period ");
  puts(itoa(p->period));
}

static void current_process_report(){
  process_report(current_node);
}

static void print_processes_list(void){
  run_queue_t * q = first_queue;
  node_t * p;
  current_process_report();
  puts("  (running) \n");
  while(q != 0){
    p = q->first;
    while(p != 0){
      process_report(p);
      puts("\n");
      p = p->next_node;
    }
    q = q->next_queue;
  }  
}

/**************************************************** Fill in the Rate Monotonic Scheduler ****************************************************/

scheduler_t rate_monotonic_scheduler = {
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


