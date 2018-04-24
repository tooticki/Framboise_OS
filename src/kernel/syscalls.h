#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "processes.h"

// OS-LEVEL INTERFACE (only for interrupts.h/.c)

typedef void (*sys_function_t) (void * r1);

extern sys_function_t syscalls_table[];

// LOW-LEVEL INTERFACE (for libc)

// List of existing syscalls ("n" below)
enum {
  SYS_PUTS,
  SYS_GETS,
  SYS_PAUSE,   // gives CPU to another process
  SYS_CREATE_PROCESS,
  syscalls_table_length,
};

// Calls syscall #n, passing it an argument
void syscall(unsigned int n, void *arg);

// Populates the syscall_table array. Should be called once at boot.
void syscalls_init(void);

// HIGH-LEVEL INTERFACE (wrappers around syscall(SYS_FOO, ...))

// Helper functions 
void user_puts(char * s);
void user_gets(char *s, unsigned int n); // n is a bound for the string length
void user_pause(void);
void user_create_process(kthread_function_f thread_func, char *name);

#endif
