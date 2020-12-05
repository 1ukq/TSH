#ifndef SHELL
#define SHELL

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "utils/fonctions_utiles.h"
//#include <readline/readline.h>
//#include <readline/history.h>
//#include <stdlib.h>

#include "list_file.h"
#include "cd.h"
#include "pwd.h"

#define PATH_MAX 4096

int read_input(char **, _Bool *);

int shell(void);

#endif
