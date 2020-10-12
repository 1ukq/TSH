#ifndef COPY
#define COPY

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BLOCK_SIZE 512

char buffer_block[BLOCK_SIZE];

int copy(const char *path_tar, const char *path_file_source, int fd_dest);

void cat(const char *path_tar, const char *path_file_source);

#endif