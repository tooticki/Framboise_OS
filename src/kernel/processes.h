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

// Thread function
typedef void (*kthread_function_f)(void);

// Process' functions
void processes_init(void);  // Initializes
process_control_block_t *  create_process(kthread_function_f thread_func, char * name, int name_len);
void reap(void);
int kill(uint32_t pid); // returns -1 if Not Found
void schedule(void);

#endif
