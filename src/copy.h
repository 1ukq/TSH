#ifndef COPY
#define COPY

#include "types/posix_header.h"
#include "utils/find_file.c"
#include "utils/init_header.c"
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
#include "utils/utils_string.c"

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);

int copy_in_tar(const char *path_file_source, const char *path_tar, const char *target);

int cat(const char *path_tar, const char *path_file_source);

#endif
