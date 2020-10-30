#ifndef INSERT
#define INSERT

#include "types/posix_header.h"
#include "init_header.h"
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

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes);

int find_next_block(int fd_tar, struct stat *restrict buf_stat);

char *buffarize(const char *path_file_source, struct stat *restrict buf);

int insert_file_in_tar(const char *path_tar, const char *path_file_source);

#endif