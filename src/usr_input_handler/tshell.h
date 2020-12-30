#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "parser.h"
#include "utils_rdr.h"
#include "cmds_launcher.h"
#include "../cd.h"

#define PATH_MAX 4096

int read_input(char * input);

int introduction(void);

int tshell(void);
