#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "fonctions_utiles.c"

#define PATH_MAX 4096


int count_cmd(char * string);

int count_words(char * string);

char *** parser(char * input, char * cwd, char * path_to_tsh);
