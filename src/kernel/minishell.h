#ifndef MINISHELL_H
#define MINISHELL_H

struct command{
  const char *name;
  void (*command)(void);
  const char *description;
};

struct info_command{
  const char *name;
  void (*info_command)(void *shell);
  const char *description;
};

typedef struct shell{
  char *prompt;
  uint32_t num_commands;
  struct info_command help;
  struct command dictionary[];
} shell_t;


void shell_exec_command(shell_t *shell, char *command);
int shell_loop(shell_t *shell);
void shell_error(void);
void run_shell(void);


#endif
