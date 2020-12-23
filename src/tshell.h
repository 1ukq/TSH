#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "utils/parser.c"
#include "utils/utils_rdr.c"

#include "utils/fonctions_utiles.c"

#define PATH_MAX 4096

int read_input(char * input);

int tshell(void);
