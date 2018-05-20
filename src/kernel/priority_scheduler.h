#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H

#include <stdint.h>

#include "processes.h"

// PCB-node
typedef struct node{
  process_control_block_t * process; // PCB itself
  unsigned int priority;             // Priority of this process
  struct node * next_node;           // The next PCB-node in its Run Queue  
} pcb_node_t;

// List(queue) of PCB-nodes
typedef struct {
  pcb_node_t * first;
  pcb_node_t * last;
} pcb_list_t;

void push_node(pcb_list_t * l, pcb_node_t * p);       // Add a pcb-node in the end of the list l
pcb_node_t * pop_node(pcb_list_t * l);                // Pop a pcb-node from the beginning of the list l
pcb_node_t * peek_node(pcb_list_t * l);               // Peek a pcb-node from the beginning of the list l
pcb_node_t * find_node(pcb_list_t * l, uint32_t pid); // Look for the process with a given pid, return 0 if not found
void remove_node(pcb_list_t * l, pcb_node_t * p);     // Remove a pcb-node from the list l (only if p was in l)

// Scheduler's functions
void scheduler_init(process_control_block_t * init_process); // Initialize the scheduler: define current process
                                                             // as init_process

                                                             // Add a process to the run queues of the scheduler
void register_process(process_control_block_t * process, unsigned int priority); 

process_control_block_t * pop_process();                     // Choose the next proc to run and removes it from
                                                             // the queues; updates current_node; returns 0 if
                                                             // the schduler doesn't want to change the process

process_control_block_t * pop_another_process();             // Choose the next proc to run and removes it from
                                                             // the queues; updates current_node; returns 0 if 
                                                             // and only if there is no more processes

void push_current_process();                                 // Put the current process in the Run Queues
int remove_by_pid(uint32_t pid);                             // Remove the process with pid from the Run Queues;
                                                             // return -1 if Not Found, return 1 otherwise

process_control_block_t * get_current_process();             // Return the PCB of the running process 
uint32_t current_pid();                                      // Return the pid of the running process

// Printing information functions
void run_queues_report(void);      // Print for each queue if it's empty or not
void print_processes_list(void);   // Display the list of running processes with their names, pids and priorities
void process_report(pcb_node_t * p); // Display the name, pid and priority of the current process
void current_process_report(void); // Display the name, pid and priority of the current process
#endif
