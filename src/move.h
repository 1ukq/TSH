#ifndef MOVE
#define MOVE

#include "types/posix_header.h"
#include "utils.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "init_header.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes);

int find_next_block(int fd_tar, struct stat *restrict buf_stat);

char *buffarize(const char *path_file_source, struct stat *restrict buf);

int insert_file_in_tar(const char *path_tar, const char *path_file_source, char *path_in_tar);

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, char *path_in_tar);

int mv_from_dir_to_tar(const char *path_tar, const char *path_file_source, char *path_in_tar);

int mv_from_tar_to_dir(const char *path_tar, const char *path_file_source, char *path_dest);

#endif