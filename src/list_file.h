#ifndef LIST_FILE
#define LIST_FILE

#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open
#include <unistd.h> //read write close
#include <string.h> //strlen strcat...
#include <stdio.h> //perror
#include <stdlib.h> //atoi
#include <time.h> //gmtime
#include "types/work_directory.h"
#include "utils/fonctions_utiles.h"
#include "types/posix_header.h"

#define BLOCK_SIZE 512
#define LTYPE "-7lcbdps"
#define LPERM "rwxrwxrwx"

int verif_convert_name(char * name, char * fullname, const char * path_cwd);

void get_type(char ptypeflag, char * type);

void oct_to_dec(char * oct, char * dec);

void dec_to_bin(char * dec, char * bin);

void oct_to_bin(char * oct, char * bin);

void get_permissions(char * perm_oct, char * perm_str);

void get_time(char * time_oct, char * time);

int ls(char option, char * path, char * cwd);

#endif
