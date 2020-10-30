#ifndef COPY
#define COPY

#include "types/posix_header.h"
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

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);

void cat(const char *path_tar, const char *path_file_source);

#endif