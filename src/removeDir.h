#ifndef REMOVEDIR
#define REMOVEDIR

#include "types/posix_header.h"
#include "utils/find_file.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define BLOCK_SIZE 512

int removeDirectory(const char *path_tar, const char *path_file_source);

#endif