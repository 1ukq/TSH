#ifndef UTILS
#define UTILS

#include <string.h>
#include <stdlib.h>

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes);

void copy_string(char *str1, char *str2, int length);

int str_length(char *str);

int name_length(const char* path_file_source);

char *name(const char *path);

char *concatenate(char *str1, char *str2);

int name_dir_length(const char *path_dir_src);

char *name_dir(const char *path);

#endif