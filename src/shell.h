#ifndef SHELL
#define SHELL

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <readline/readline.h>
//#include <readline/history.h>
//#include <stdlib.h>

#define PATH_MAX 4096

int read_input(char **);

int shell(void);

#endif
