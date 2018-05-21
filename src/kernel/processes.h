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
  process_saved_state_t * saved_state; // Pointer to where on the stack this process' state is saved; becomes invalid once the process is running
  void * stack_page;                   // The stack for this proces, it starts at the end of this page
  uint32_t pid;                        // The process ID number
  char process_name[20];               // The process' name
} process_control_block_t;

// Scheduling tools

typedef struct {
  void (*scheduler_init)(process_control_block_t * init_process);          // Initialize the scheduler: define current process
  void (*register_process)(process_control_block_t * process, void * arg); // Add a process to the run queues of the scheduler;
  
  process_control_block_t * (*pop_process)(void);         // Choose the next proc to run and removes it from
                                                          // the queues; updates current_node; returns 0 if
                                                          // the schduler doesn't want to change the process
  
  process_control_block_t * (*pop_another_process)(void); // Choose the next proc to run and removes it from
                                                          // the queues; clears its node; updates current_node;
                                                          // returns 0 if and only if there is no more processes
                                                          // Called only by processes:reap()

  void (*push_current_process)(void);                     // Put the current process in the Run Queues
  int (*remove_by_pid)(uint32_t pid);                     // Remove the process with the given pid from the Run Queues;

  process_control_block_t * (*get_current_process)(void); // Return the PCB of the running process
  uint32_t (*current_pid)(void);                          // Return the pid of the running process
  
  void (*current_process_report)(void);                   // Display the name, pid and other info about the current process
  void (*print_processes_list)(void);                     // Display the list of running processes with their info
} scheduler_t;

// It;s initialized in processes_init
extern scheduler_t * scheduler; // TODO: MOVE?

// Thread function
typedef void (*kthread_function_f)(void);

// Processes' functions

// Values of scheduler_type in process_init(unsigned int scheduler_type)
enum{
  RATE_MONOTONIC,
  PRIORITY,
};

void processes_init(unsigned int scheduler_type); // Initialise the main process, with a given scheduler call scheduler_init

                           // Create a process_saved_state and a PCB and fills it
process_control_block_t * create_process(kthread_function_f thread_func, char * name, int name_len);

void reap(void);           // Kill the currently running process;
			   // loop forever if there are no more processes

int kill(uint32_t pid);    // Kill the process with a given pid;
			   // return -1 if Not Found, return 1 otherwise

void schedule(void);       // Choosing the next process to run: call pop_process(),
                           // push_current_process() and switch the context

#endif
