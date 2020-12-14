#ifndef REMOVE
#define REMOVE

#include "types/posix_header.h"
#include "utils/find_file.h"
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

int remove_file_from_tar(const char *path_tar, const char *path_file_source);
int remove_file_from_tar_r(const char *path_tar, const char *path_file_source);

#endif