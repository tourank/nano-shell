#ifndef NANO_SHELL_H
#define NANO_SHELL_H
void nano_loop(void);
char *nano_read_line(void);
char **nano_split_line(char*);
int nano_launch(char **);
int nano_execute(char **);

int nano_cd(char **);
int nano_help(char **);
int nano_exit(char **);
int nano_num_builtins();

int nano_cd(char **args);
int nano_help(char **args);
int nano_exit(char **args);


#endif
