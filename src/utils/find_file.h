#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include "../types/posix_header.h"
#include "init_header.h"
#include "utils_string.h"

int check_sys_call(int ret_sys_call, char *err_msg);

int pos_file_in_tar(int fd_tar, const char *path_in_tar, int *pos);

int suppress_file(int fd_tar, int pos_from, int pos_to, int size_tar);

char **get_sub_dirs(int fd_tar, const char *path_dir_src);