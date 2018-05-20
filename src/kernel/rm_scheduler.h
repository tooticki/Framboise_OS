#ifndef RM_SCHEDULER_H
#define RM_SCHEDULER_H

#include <stdint.h>

#include "processes.h"

// PCB-node
typedef struct node_rm{
  process_control_block_t * process; // PCB itself
  uint32_t runtime;                  // Run time of this process, in microseconds
  uint32_t period;                   // Period of this process, in microseconds
  struct node_rm * next_node;           // The next node in its Run Queue  
} node_rm_t;

// Queue of PCB-nodes
typedef struct run_queue{
  uint32_t period;
  node_rm_t * first;
  node_rm_t * last;
  struct run_queue * next_queue;
} run_queue_t;

void push_node_rm(run_queue_t * q, node_rm_t * p);       // Add a node in the end of the queue q
node_rm_t * pop_node_rm(run_queue_t * q);                // Pop a node from the beginning of the queue q
node_rm_t * peek_node_rm(run_queue_t * q);               // Peek a node from the beginning of the queue q
node_rm_t * find_node_rm(run_queue_t * q, uint32_t pid); // Look for the process with a given pid, return 0 if not found
void remove_node_rm(run_queue_t * q, node_rm_t * p);     // Remove a node from the queue q (only if p was in q)

// List of queues
void new_queue(run_queue_t * q, node_rm_t * p); // Allocate memory for q, make p its single element
void add_to_queues(node_rm_t * p);              // Put p into a queue with the appropriate deadline
void remove_queue(run_queue_t * q);                // Remove q from the list of queues

// RM-scheduler's functions
void rm_scheduler_init(process_control_block_t * init_process); // Initialize the scheduler: define current process
                                                             // as init_process

                                                             // Add a process to the run queues of the scheduler;
                                                             // its period and runtime are stored in args
void rm_register_process(process_control_block_t * process, void * arg); 

process_control_block_t * rm_pop_process();                     // Choose the next proc to run and removes it from
                                                             // the queues; updates current_node; returns 0 if
                                                             // the schduler doesn't want to change the process

process_control_block_t * rm_pop_another_process();             // Choose the next proc to run and removes it from
                                                             // the queues; clears its node; updates current_node;
                                                             // returns 0 if and only if there is no more processes
                                                             // Called only by processes:reap()

void rm_push_current_process();                                 // Put the current process in the Run Queues
int rm_remove_by_pid(uint32_t pid);                             // Remove the process with pid from the Run Queues;
                                                             // return -1 if Not Found, return 1 otherwise

process_control_block_t * rm_get_current_process();             // Return the PCB of the running process 
uint32_t rm_current_pid();                                      // Return the pid of the running process

// Printing information functions
void rm_run_queues_report(void);        // Print for each queue its period
void rm_process_report(node_rm_t * p); // Display the name, pid, runtime and period of the process
void rm_current_process_report();      // Display the name, pid, runtime and period of the current process
void rm_print_processes_list(void);     // Display the list of running processes with their info

#endif
