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
  struct pcb * next_pcb;               // The next PCB in the Run Queue
} process_control_block_t;

// PCB List
typedef struct {
  process_control_block_t * first;
  process_control_block_t * last;
} pcb_list_t;

//pcb_list_t pcb_list_init(void);
void push_pcb(pcb_list_t * l, process_control_block_t * p);       // Adds a pcb in the end of the list l
process_control_block_t * pop_pcb(pcb_list_t * l);                // Pops a pcb from the beginning of the list l
process_control_block_t * peek_pcb(pcb_list_t * l);               // Peeks a pcb from the beginning of the list l
process_control_block_t * find_pcb(pcb_list_t * l, uint32_t pid); // Looks for the pcb with a given pid, returns 0 if not found
void remove_pcb(pcb_list_t * l, process_control_block_t * p);     // Removes a pcb from the list l (only if p was in l)

// Thread function
typedef void (*kthread_function_f)(void);

// Process' functions
void processes_init(void);
void create_process(kthread_function_f thread_func, unsigned int priority, char * name, int name_len);
void reap(void);
int kill(uint32_t pid); // returns -1 if Not Found
void schedule(void);

void run_queues_report(void);
void process_report(void);
#endif
