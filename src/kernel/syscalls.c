#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "interrupts.h"
#include "processes.h"
#include "priority_scheduler.h"
#include "syscalls.h"

sys_function_t syscalls_table[100];


// Calls syscall #n, passing it an argument
void syscall(unsigned int n, void *arg){ // arguments are already in r0 and r1 ;-)
  (void)n;
  puts("");
  __asm__ __volatile__ ("mov r1, %0; SWI #0 " : :"g" (arg)); // moves arg in r1, runs SWI

}

// Functions implementing system calls
void sys_puts(void *s){
  puts((char *) s);
}

typedef struct {
  char *s;
  unsigned int n;
} gets_args_t;

void sys_gets(void *args){
  gets_args_t * tmp = args;
  gets(tmp->s, tmp->n);
}
void sys_pause(void * unused){
  (void)unused;
  schedule();
}

typedef struct {
  kthread_function_f thread_func;
  int priority;
  char *name;
  int name_len;
} create_process_args_t;

void sys_create_process(void * args){
  create_process_args_t * tmp = args;
  register_process(create_process(tmp->thread_func, tmp->name, tmp->name_len), tmp->priority); //TODO: Should be changed 
}

// Populates the syscalls_table array. Should be called once at boot
void syscalls_init(void){
  syscalls_table[SYS_PUTS] = sys_puts;
  syscalls_table[SYS_GETS] = sys_gets;
  syscalls_table[SYS_PAUSE] = sys_pause;
  syscalls_table[SYS_CREATE_PROCESS] = sys_create_process;
}


// HIGH-LEVEL INTERFACE (wrappers around syscall(SYS_FOO, ...))

// Helper functions 
void user_puts(char * s){
  syscall(SYS_PUTS, s);
}

void user_gets(char *s, unsigned int n){
  gets_args_t args = {s, n};
  syscall(SYS_GETS, &args);
}

void user_pause(void){
  syscall(SYS_PAUSE, 0);
}

void user_create_process(kthread_function_f thread_func, int priority, char *name){
  create_process_args_t args = {thread_func, priority, name, strlen(name)};
  syscall(SYS_CREATE_PROCESS, &args);
}
