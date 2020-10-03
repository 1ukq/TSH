#include "copy.h"
#include "types/posix_header.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int copy(const char *path_tar, const char *path_file_source, const char *path_file_dest){

    ssize_t size_read;
    ssize_t size_write;
    int size = 0;
    struct posix_header p;

    int fd_source = open(path_tar, O_RDONLY);
    if(fd_source == -1){
        perror("open");
        return -1;
    }

    int fd_dest = open(path_file_dest, O_WRONLY|O_CREAT, 0000700);
    if(fd_dest == -1){
        perror("open");
        return -1;
    }

    int shift = 0;

    while(strcmp(p.name, path_file_source)){

        size_read = read(fd_source, &p, BLOCK_SIZE);
        if(size_read == -1){
            perror("read");
            return -1;
        }

        sscanf(p.size, "%x", &size);

        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    }

    char *buf = malloc(shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc");
        return -1;
    }

    size_read = read(fd_source, buf, shift * BLOCK_SIZE);
    if(size_read == -1){
        perror("read");
        return -1;
    }

    int nb_char_in_buf = 0;
    for(int i = 0; i < shift * BLOCK_SIZE; i++){
        if(buf[i] == '\0'){
            nb_char_in_buf = i;
            break;
        }
    }

    size_write = write(fd_dest, buf, nb_char_in_buf);
    if(size_write == -1){
        perror("write");
        return -1;
    }

    close(fd_source);
    close(fd_dest);
    free(buf);
    
    return nb_char_in_buf;

}