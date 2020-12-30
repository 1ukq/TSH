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
#include "utils/fonctions_utiles.h"

#define BLOCK_SIZE 512

int cd(char * path_cwd, char * path_nwd);

#endif
