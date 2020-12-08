#include "utils_red.h"

int pos_file_in_tar(int fd_tar, const char *path_in_tar, int *pos){
    struct posix_header header;
    int r = read(fd_tar, &header, BLOCK_SIZE);
    int size = 0;
    int shift = 0;
    while(strcmp(header.name, path_in_tar)){

        if(header.name[0] == '\0'){
            printf("No such file\n");
            return -1;
        }

        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
        int ret_lseek = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
        if(ret_lseek == -1){
            perror("lseek in pos_file_in_tar");
            return -1;
        }

        r = read(fd_tar, &header, BLOCK_SIZE);
        if(r == -1){
            perror("read in pos_file_in_tar");
            return -1;
        }

    }
    sscanf(header.size, "%o", &size);
    int pos_b = lseek(fd_tar, 0, SEEK_CUR);
    if(pos_b == -1){
        perror("lseek in pos_file_in_tar");
        return -1;
    }
    *(pos) = pos_b;
    *(pos + 1) = pos_b + size;
    return 0;
}

int size_file(fd_tar, pos_from){}