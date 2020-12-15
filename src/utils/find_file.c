#include "find_file.h"

int check_sys_call(int ret_sys_call, char *err_msg){
    if(ret_sys_call == -1){
        perror(err_msg);
        return -1;
    }
    return 0;
}

int pos_file_in_tar(int fd_tar, const char *path_in_tar, int *pos){
    struct posix_header header;
    int r = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(r, "read in pos_file_in_tar") == -1) return -1;
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
        if(check_sys_call(ret_lseek, "lseek in pos_file_in_tar") == -1) return -1;

        r = read(fd_tar, &header, BLOCK_SIZE);
        if(check_sys_call(r, "read in pos_file_in_tar") == -1) return -1;

    }
    sscanf(header.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    int pos_b = lseek(fd_tar, 0, SEEK_CUR);
    if(check_sys_call(pos_b, "lseek in pos_file_in_tar") == -1) return -1;
    *(pos) = pos_b;
    *(pos + 1) = pos_b + (shift * BLOCK_SIZE);
    *(pos + 2) = pos_b + size;
    return 0;
}

int suppress_file(int fd_tar, int pos_from, int pos_to, int size_tar){

    char *buf_sup = malloc(sizeof(char) * (size_tar - pos_from));
    if(buf_sup == NULL){
        perror("malloc in suppress_file");
        return -1;
    }
    memset(buf_sup, '\0', sizeof(char) * (size_tar - pos_from));

    int ret_lseek = lseek(fd_tar, pos_to, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in suppress_file") == -1) return -1;

    int size_read = read(fd_tar, buf_sup, sizeof(char) * (size_tar - pos_to));
    if(check_sys_call(size_read, "read in suppress_file") == -1) return -1;

    ret_lseek = lseek(fd_tar, pos_from, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in suppress_file") == -1) return -1;

    int size_write = write(fd_tar, buf_sup, size_tar - pos_from);
    if(check_sys_call(size_write, "write in suppress_file") == -1) return -1;

    free(buf_sup);

    return 0;

}