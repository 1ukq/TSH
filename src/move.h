#ifndef MOVE
#define MOVE

#include "types/posix_header.h"
#include "utils/utils_string.h"
#include "utils/find_file.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "utils/init_header.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>
#include "types/work_directory.h"
#include "utils/fonctions_utiles.h"

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes);

int find_last_block(int fd_tar);

char *buffarize(const char *path_file_source, struct stat *restrict buf);

int insert_file_in_tar(const char *path_tar, const char *path_file_source, char *path_in_tar);

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, char *path_in_tar);

int mv_from_dir_to_tar(const char *path_tar, const char *path_file_source, char *path_in_tar);

int mv_from_tar_to_dir(const char *path_tar, const char *path_file_source, char *path_dest);

int mv(char * path_file_source, char * path_file_dest);

#endif
