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
#include <uuid/uuid.h>
#include <time.h>

char buffer_block[BLOCK_SIZE];

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);

void cat(const char *path_tar, const char *path_file_source);

char *buffarize(const char *path_file_source, struct stat *restrict buf);

void init_type_file(struct stat *restrict buf, struct posix_header *header);

void init_mode_file(struct stat *restrict buf, struct posix_header *header);

int create_file_header(const char *file_name, struct stat *restrict buf, struct posix_header *header);

void init_uid_file(struct stat *restrict buf, struct posix_header *header);

void init_gid_file(struct stat *restrict buf, struct posix_header *header);

void insert_file_in_tar(const char *path_tar, const char *path_file_source);

#endif