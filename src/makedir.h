#ifndef MAKEDIR
#define MAKEDIR

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include "types/posix_header.h"
#include "types/work_directory.h"
#include "utils/fonctions_utiles.h"
#include "utils/init_header.h"
#include "utils/utils_string.h"


#define BLOCK_SIZE 512

void get_prev_rep(char * path, char * path_prev_rep);

int init_header_dir(char * path_in_tar, struct posix_header *p);

int makedir(char * path);


#endif
