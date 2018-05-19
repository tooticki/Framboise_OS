#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include <stdint.h>

#include "processes.h"

// PCB Node
typedef struct node{
  process_control_block_t * process;
  unsigned int priority;
  struct node * next_node;               // The next PCB in the Run Queue  
} pcb_node_t;

// List of PCB-nodes
typedef struct {
  pcb_node_t * first;
  pcb_node_t * last;
} pcb_list_t;

void push_node(pcb_list_t * l, pcb_node_t * p);       // Add a pcb in the end of the list l
pcb_node_t * pop_node(pcb_list_t * l);                // Pop a pcb from the beginning of the list l
pcb_node_t * peek_node(pcb_list_t * l);               // Peek a pcb from the beginning of the list l
pcb_node_t * find_node(pcb_list_t * l, uint32_t pid); // Look for the pcb with a given pid, return 0 if not found
void remove_node(pcb_list_t * l, pcb_node_t * p);     // Remove a pcb from the list l (only if p was in l)

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
