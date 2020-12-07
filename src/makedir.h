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


#define BLOCK_SIZE 512

void set_checksum(struct posix_header *hd);

void get_prev_rep(char * path, char * path_prev_rep);

int makedir(char * path);


#endif
