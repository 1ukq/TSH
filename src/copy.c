#include "copy.h"

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest){

    if(fd_dest == -1){
        return -1;
    };

    ssize_t size_read;
    ssize_t size_write;
    int size = 0;
    struct posix_header p;

    int fd_source = open(path_tar, O_RDONLY);
    if(fd_source == -1){
        perror("open in copy_from_tar");
        return -1;
    }

    int shift = 0;

    size_read = read(fd_source, &p, BLOCK_SIZE);
    if(size_read == -1){
        perror("read in copy_from_tar");
        return -1;
    }

    while(strcmp(p.name, path_file_source)){

        if(p.name[0] == '\0'){
            printf("No such file\n");
            return -1;
        }

        sscanf(p.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
        int ret_lseek = lseek(fd_source, shift * BLOCK_SIZE, SEEK_CUR);
        if(ret_lseek == -1){
            perror("lseek in copy_from_tar");
            return -1;
        }

        size_read = read(fd_source, &p, BLOCK_SIZE);
        if(size_read == -1){
            perror("read in copy_from_tar");
            return -1;
        }

    }

    sscanf(p.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in copy_from_tar");
        return -1;
    }

    size_read = read(fd_source, buf, shift * BLOCK_SIZE);
    if(size_read == -1){
        perror("read in copy_from_tar");
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
        perror("write in copy_from_tar");
        return -1;
    }

    close(fd_source);
    free(buf);
    
    return nb_char_in_buf;

}

void cat(const char *path_tar, const char *path_file_source){

    int n = copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    if(n == 1) perror("");

}

