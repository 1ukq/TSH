#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include "../types/posix_header.h"
#include "../utils/init_header.h"

int pos_file_in_tar(int fd_tar, const char *path_in_tar, int *pos);