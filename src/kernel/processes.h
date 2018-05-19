#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>

// PCB (Process Control Block)
typedef struct {
  uint32_t r0;
  uint32_t r1; 
  uint32_t r2; 
  uint32_t r3; 
  uint32_t r4; 
  uint32_t r5; 
  uint32_t r6; 
  uint32_t r7; 
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t cpsr; 
  uint32_t sp;
  uint32_t lr;
  uint32_t old_stack[];
} process_saved_state_t;

typedef struct pcb {
  process_saved_state_t * saved_state; // Pointer to where on the stack this process' state is saved. Becomes invalid once the process is running
  void * stack_page;                   // The stack for this proces.  The stack starts at the end of this page
  uint32_t pid;                        // The process ID number
  char process_name[20];               // The process' name
} process_control_block_t;

// PCB List
typedef struct node{
  process_control_block_t * process;
  unsigned int priority;
  struct node * next_node;               // The next PCB in the Run Queue  
} pcb_node_t;

typedef struct {
  pcb_node_t * first;
  pcb_node_t * last;
} pcb_list_t;

//pcb_list_t pcb_list_init(void);
void push_node(pcb_list_t * l, pcb_node_t * p);       // Add a pcb in the end of the list l
pcb_node_t * pop_node(pcb_list_t * l);                // Pop a pcb from the beginning of the list l
pcb_node_t * peek_node(pcb_list_t * l);               // Peek a pcb from the beginning of the list l
pcb_node_t * find_node(pcb_list_t * l, uint32_t pid); // Look for the pcb with a given pid, return 0 if not found
void remove_node(pcb_list_t * l, pcb_node_t * p);     // Remove a pcb from the list l (only if p was in l)

// Thread function
typedef void (*kthread_function_f)(void);

// Process' functions
void processes_init(void);
process_control_block_t *  create_process(kthread_function_f thread_func, char * name, int name_len);
void reap(void);
int kill(uint32_t pid); // returns -1 if Not Found
void schedule(void);

// Scheduler's functions
void scheduler_init(process_control_block_t * init_process);                     // Initialize the scheduler
void register_process(process_control_block_t * process, unsigned int priority); // Add a process to the scheduler

process_control_block_t * pop_process();                                         // Choose the next proc to run and removes it from
                                                                                 // the queue; updates current_node; returns 0 if
                                                                                 // the schduler doesn't want to change the process

process_control_block_t * pop_another_process();                                 // Choose the next proc to run and removes it from
                                                                                 // the queue; updates current_node; returns 0 if 
                                                                                 // and only if there is no more processes
void push_current_process();
int remove_by_pid(uint32_t pid);
process_control_block_t * get_current_process();
uint32_t current_pid();

// Printing information functions
void run_queues_report(void);
void print_processes_list(void);
void process_report(void);
#endif
