#ifndef UTILS
#define UTILS

#include <string.h>
#include <stdlib.h>

int str_length(char *str);

int name_length(const char* path_file_source);

char *name(const char *path);

char *concatenate(char *str1, char *str2);

#endif