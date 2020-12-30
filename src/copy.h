#ifndef COPY
#define COPY

#include "types/posix_header.h"
#include "utils/find_file.h"
#include "utils/init_header.h"
#include "utils/check_dir.h"
#include "utils/utils_string.h"
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
#include <dirent.h>

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);

int copy_in_tar(const char *path_file_source, const char *path_tar, const char *target);

int copy_from_tar_r(const char *path_tar_source, const char *path_dir, const char *path_file_dest);

int copy_in_tar_r(const char *path_tar, const char *file_dest, char *path_dir);

int cat(const char *path_tar, const char *path_file_source);

int copy_from_tar_to_tar(const char *path_tar_src, const char* path_tar_dest, const char *file_src, const char *file_dest);

int copy_from_tar_to_dir_r(const char *path_tar, const char *path_dir_src, char *path_dir_dest);

int build_tree_structure(int fd_tar, const char *path_dir_src, char *path_dir_dest);

int copy_from_tar_to_tar_r(const char *path_tar_src, const char *path_dir_src, char *path_tar_dest, char *path_dir_dest);

#endif
