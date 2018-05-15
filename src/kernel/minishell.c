#include "../common/stdio.h"
#include "../common/stdlib.h"

#include "processes.h"
#include "timer.h"
#include "minishell.h"

void shell_help(){
  puts("\nTo create a process: H <process name> <piority>\n");
  puts("\nTo kill a process: K <process pid>\n");
  puts("\nTo get the list of all processes: L\n");
  puts("\nTo get help: H\n");
}

void test_process(){
  int i = 0;
  while (i<5) {
    process_report();
    puts(itoa(i++));
    puts("\n");
    udelay(1000000); // 1 sec
  }
}

void shell_create_process(char* s){
  int i = 0, name_start, priority_start, name_len, priority;
  while(s[i] == ' ')
    i++;
  name_start = i;
  while(s[i] != ' ' && s[i] != '\0')
    i++;
  if(s[i] == '\0'){
    shell_error();
    shell_help();
    return;
  }
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
}
void shell_kill_process(char* s){
  if(s[0] < '0' || s[0] > '9'){
    shell_error();
    shell_help();
    return;
  }
  if(kill(atoi(s)) == -1)
    puts("\nShell error: No such process\n");
}

void shell_list(void){
  shell_error();
}

void shell_error(){
  puts("\nShell error: Invalid command\n");
}

void parse(char* s){
  int i = 0;
  while(s[i] == ' ')
    i++;
  switch(s[i]){
  case 'C':
    shell_create_process(s+i);
    break;
  case 'K':
    shell_kill_process(s+i);
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
  char buf[64];
  while(1){
    puts("user@framboise☺ $ ");
    gets(buf, 32);
    parse(buf);    
  }
  
}
