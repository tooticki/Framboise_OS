#include "../common/stdlib.h"
#include "../common/stdio.h"
#include "../common/string.h"

#include "timer.h"
#include "processes.h"
#include "priority_scheduler.h"
#include "minishell.h"


/* *************************** Shell Functions ************************************* */


void shell_exec_command(shell_t *shell, char *command){
  char *command_name = strtok_(command, " ");

  if(strcmp_(command_name, shell->help.name) == 0){ //help case
    shell->help.info_command((void *)shell);
    return;
    }
  for(uint32_t i = 0; i < shell->num_commands; i++) //help is always 0
    if(strcmp_(command_name, shell->dictionary[i].name) == 0){
      shell->dictionary[i].command();
      return;
    }
  
  puts("Shell error: command not found\n");
}

int status;

int shell_loop(shell_t *shell){
  char input_buffer[200];
  status = 0;
  while(status == 0){
    puts(shell->prompt);
    puts("@framboise☺ $ ");
    read_line(input_buffer, 200);
    char *command_name = strtok_(input_buffer, "\n");
    shell_exec_command(shell, command_name);
  }
  return status;
}

void shell_error(){
  puts("Shell error: Invalid command, use H for help\n"); 
}

/* ****************************** Commands ********************************** */


void echo(void){
  /* Use strtok(0, " ") to get parameters.
     Returns 0 when no more parameters. */
  char *s;

  while((s = strtok_(0, " ")) != 0)
    puts(s);
  puts("\n");
}

void sum(void)
{
  char *s;
  int result = 0;

  while((s = strtok_(0, " ")) != 0)
    result += atoi(s);

  puts(itoa(result));
  puts("\n");
}

void quit(void){
  char *s;
  int n;

  if((s = strtok_(0, " ")) != 0)
    n = atoi(s);
  else{
    puts("Usage: exit <n>\n");
    return;
  }  
  if(n < 2)
    puts("Exit status should be > 1\n");
  else
    status = n;
}

void switch_shell(void){
  char *s;
  s = strtok_(0, " ");
  if(s == 0)
    puts("Error: choose a shell to switch to\n");
  else{
    if(strcmp_(s, "priority_shell") == 0){
      status = -1;
      return;
    }
    if(strcmp_(s, "real_time_shell") == 0){
      status = 1;
      return;
    }
    puts("Error: no shell called ");
    puts(s);
    puts("\n");
  }  
}

void help(void *s){
  shell_t * shell = (shell_t *)s;
  puts(shell->help.name);
  puts(" ");
  puts(shell->help.description);
  puts("\n");
  for(uint32_t i = 0; i < shell->num_commands; i++){
    puts(shell->dictionary[i].name);
    puts(" ");
    puts(shell->dictionary[i].description);
    puts("\n");
  }
}

/* ****************************** Shells ********************************** */

shell_t priority_shell = {
  "priority_shell",
  4,
  {"help", help, " - Dipslay this information"},
  {
    {"echo", echo, " - Echo characters"},
    {"sum", sum, "<n_1 n_2 ...> - Computes sum of the following numbers"},
    {"switch", switch_shell, "<name> - Switch to the shell name"},
    {"exit", quit, "<n> - Exit with status n > 1"},
  },
};

shell_t real_time_shell = {
  "real_time_shell",
  3,
  {"help", help, " - Dipslay this information"},
  {
    {"echo", echo, " - Echo characters"},
    {"sum", sum, "<n_1 n_2 ...> - Computes sum of the following numbers"},
    {"switch", switch_shell, "<name> - Switch to the shell name"},
    {"exit", quit, "<n> - Exit with status n > 1"},
  },
};


void run_shell(void){
  puts("\nWelcome to the Framboise priority mini-shell!\n");
  status = shell_loop(&priority_shell);
  while(status < 2){
    puts("\nWelcome to the Framboise ");
    switch(status){
    case -1:
      puts("priority mini-shell!\n");
      status = shell_loop(&priority_shell);
      break;
    case 1:
      puts("real time mini-shell!\n");
      status = shell_loop(&real_time_shell);
      break;
    }
  }
  puts("Shells terminated with status ");
  puts(itoa(status));
  puts("\n");
}



/* ****************************** Other funcitons ********************************** */


void test_process(){
  int i = 5;
  while (i>0) {
    scheduler->current_process_report();
    puts(itoa(--i));
    puts("\n");
    udelay(5000000); // 5 sec
  }
}

void shell_create_process(char* s){
  int i = 0, name_start, priority_start, name_len, priority;
  while(s[i] == ' ')
    i++;
  name_start = i-1;
  while(s[i] != ' ' && s[i] != '\0')
    i++;
  if(s[i] == '\0'){
    shell_error();
    return;
  }
  s[i] = '\0';
  s++;
  name_len = i-name_start;
  while(s[i] == ' ')
    i++;
  if(s[i] < '0' || s[i] > '9'){
    shell_error();
    return;
  }
  priority_start = i;
  priority = atoi(s+priority_start);
  scheduler->register_process(create_process(test_process, s+name_start, name_len), (void *) (&priority)); ///TODO: add possibility to create without registering
  puts("Process ");
  puts(s+name_start);
  puts(" was succesfully created\n");
}

void shell_kill_process(char* s){
  int i = 0;
  while(s[i] == ' ')
    i++;
  if(s[i] < '0' || s[i] > '9'){
    shell_error();
    return;
  }
  if(kill(atoi(s+i)) == -1){
    puts("Shell error: No such process, ");
    puts(s+i);
    puts("\n");
    return;
  }
  puts("Process with pid ");
  puts(s+i);
  puts(" was succesfully killed\n");
}

void shell_list(void){
  scheduler->print_processes_list();
}


