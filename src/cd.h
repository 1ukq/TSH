#ifndef CD
#define CD

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "types/posix_header.h"
#include "types/work_directory.h"

#define BLOCK_SIZE 512

void get_wd(struct work_directory wd, char * path_wd);

void fill_wd(char * path_wd, struct work_directory * ad_wd);

int cd(char * path_cwd, char * path_nwd);

#endif
