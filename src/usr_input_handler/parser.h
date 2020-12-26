#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../utils/fonctions_utiles.h"

#define PATH_MAX 4096


int count_cmd(char * string);

int count_words(char * string);

char *** parser(char * input, char * cwd, char * path_to_tsh);
