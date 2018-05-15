#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "processes.h"
#include "timer.h"
#include "minishell.h"

void shell_help(){
  puts("Possible commands:\n");
  puts("  C <process name> <priority>  Create a process\n");
  puts("  K <process pid>              Kill a process\n");
  puts("  L                            Display the list of all processes\n");
  puts("  H                            Dipslay this information\n");
}

void test_process(){
  int i = 0;
  while (i<10) {
    //process_report();
    //puts(itoa(i++));
    //puts("\n");
    i++;
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
    shell_help();
    return;
  }
  priority_start = i;
  priority = atoi(s+priority_start);
  create_process(test_process, priority, s+name_start, name_len);
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
  print_processes_list();
}

void shell_error(){
  puts("Shell error: Invalid command, use H for help"); 
}

void parse(char* s){
  int i = 0;
  while(s[i] == ' ')
    i++;
  switch(s[i]){
  case 'C':
    shell_create_process(s+i+1);
    break;
  case 'K':
    shell_kill_process(s+i+1);
    break;
  case 'L':
    shell_list();
    break;
  case 'H':
    shell_help();
    break;
  case '\0':
    break;
  default:
    shell_error();
  }
}

void run_shell(void){
  char buf[128];
  while(1){
    puts("user@framboise☺ $ ");
    gets(buf, 32);
    parse(buf);    
  }
  
}
