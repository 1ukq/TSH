#include "copy.h"
#include "types/posix_header.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void *copy(const char *path_tar, const char *path_file_source){

    ssize_t size_read;
    int size = 0;
    struct posix_header p;

    int fd_source = open(path_tar, O_RDONLY);
    if(fd_source == -1) perror("open");

    int shift = 0;

    while(strcmp(p.name, path_file_source)){

        size_read = read(fd_source, &p, BLOCK_SIZE);
        if(size_read == -1) perror("read");

        sscanf(p.size, "%x", &size);

        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    }

    void *buf = malloc(shift * BLOCK_SIZE);
    if(buf == NULL) perror("malloc");

    size_read = read(fd_source, buf, shift * BLOCK_SIZE);
    if(size_read == -1) perror("read");
    
    return buf;

}