#ifndef INIT_HEADER
#define INIT_HEADER

#include "../types/posix_header.h"
#include "utils_string.h"
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

void set_checksum(struct posix_header *hd);

int check_checksum(struct posix_header *hd);

void init_type_file(struct stat *restrict buf, struct posix_header *header);

void init_mode_file(struct stat *restrict buf, struct posix_header *header);

void init_size_file(struct stat *restrict buf, struct posix_header *header);

void init_uid_file(struct stat *restrict buf, struct posix_header *header);

void init_gid_file(struct stat *restrict buf, struct posix_header *header);

void init_mtime_file(struct stat *restrict buf, struct posix_header *header);

void init_name_file(struct posix_header *header, const char *path_file_source, char *dest);

void init_magic_file(struct posix_header *header);

void init_version_file(struct posix_header *header);

int create_file_header(const char *file_name, struct stat *restrict buf, struct posix_header *header, char *dest);

#endif