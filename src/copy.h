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

char buffer_block[BLOCK_SIZE];

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);

void cat(const char *path_tar, const char *path_file_source);

char file_type(struct stat *restrict buf);

int file_mode(struct stat *restrict buf);

void create_file_header(const char *restrict path_file_source, const char *file_name, struct stat *restrict buf, struct posix_header *header);

#endif