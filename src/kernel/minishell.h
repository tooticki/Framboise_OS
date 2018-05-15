#ifndef MINISHELL_H
#define MINISHELL_H

void run_shell(void);
void parse(char* s);

void shell_help(void);
void shell_create_process(char* s);
void shell_kill_process(char* s);
void shell_list(void);
void shell_error(void);
#endif
