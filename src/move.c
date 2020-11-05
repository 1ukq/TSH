#include "move.h"

int suppress_file(int fd_tar, int pos_from, int pos_to, int size_tar){

    char *buf_sup = malloc(sizeof(char) * (size_tar - pos_from));
    if(buf_sup == NULL){
        perror("malloc in suppress_file");
        return -1;
    }
    memset(buf_sup, '\0', sizeof(char) * (size_tar - pos_from));

    int ret_lseek = lseek(fd_tar, pos_to, SEEK_SET);
    if(ret_lseek == -1){
        perror("lseek in suppress_file");
        return -1;
    }

    int size_read = read(fd_tar, buf_sup, sizeof(char) * (size_tar - pos_to));
    if(size_read == -1){
        perror("read in suppress_file");
        return -1;
    }

    ret_lseek = lseek(fd_tar, pos_from, SEEK_SET);
    if(ret_lseek == -1){
        perror("lseek in suppress_file");
        return -1;
    }

    int size_write = write(fd_tar, buf_sup, size_tar - pos_from);
    if(size_write == -1){
        perror("write in suppress_file");
        return -1;
    }

    free(buf_sup);

    return 0;

}

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, const char *path_dest){

    int fd_source = open(path_tar_source, O_RDWR);
    if(fd_source == -1){
        perror("open in mv_from_tar_to_tar");
        return -1;
    }

    int fd_target = open(path_tar_target, O_RDWR);
    if(fd_target == -1){
        perror("open in mv_from_tar_to_tar");
        return -1;
    }

    int size_tar = lseek(fd_source, 0, SEEK_END);
    if(size_tar == -1){
        perror("lseek in mv_from_tar_to_tar");
        return -1;
    }

    int pos = lseek(fd_source, 0, SEEK_SET);
    if(pos == -1){
        perror("lseek in mv_from_tar_to_tar");
        return -1;
    }

    int size_read = 0;
    int size = 0;
    int shift = 0;
    int ret_lseek = 0;
    struct posix_header header;

    size_read = read(fd_source, &header, BLOCK_SIZE);
    if(size_read == -1){
        perror("read in mv_from_tar_to_tar");
        return -1;
    }

    while(strcmp(header.name, path_file_source)){

        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

        ret_lseek = lseek(fd_source, shift * BLOCK_SIZE, SEEK_CUR);
        if(ret_lseek == -1){
            perror("lseek in mv_from_tar_to_tar");
            return -1;
        }

        size_read = read(fd_source, &header, BLOCK_SIZE);
        if(size_read == -1){
            perror("read in mv_from_tar_to_tar");
            return -1;
        }

    }

    sscanf(header.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * (shift + 1) * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in mv_from_tar_to_tar");
        return -1;
    }

    ret_lseek = lseek(fd_source, -BLOCK_SIZE, SEEK_CUR);
    if(ret_lseek == -1){
        perror("lseek in mv_from_tar_to_tar");
        return -1;
    }

    size_read = read(fd_source, buf, (shift + 1) * BLOCK_SIZE);
    if(size_read == -1){
        perror("read in mv_from_tar_to_tar");
        return -1;
    }

    int pos_from = ret_lseek;
    int pos_to = pos_from + (shift + 1) * BLOCK_SIZE;
    int ret_sup = suppress_file(fd_source, pos_from, pos_to, size_tar);
    if(ret_sup == -1){
        return -1;
    }

    struct stat stat_target;
    int check_stat = stat(path_tar_source, &stat_target);
    if(check_stat == -1){
        perror("stat in mv_from_tar_to_tar");
        return -1;
    }
    int b = find_next_block(fd_target, &stat_target);
    printf("b : %d\n", b);
    ret_lseek = lseek(fd_target, b * BLOCK_SIZE, SEEK_SET);
    if(ret_lseek == -1){
        perror("lseek in mv_from_tar_to_tar");
        return -1;
    }

    int size_write = write(fd_target, buf, (shift + 1) * BLOCK_SIZE);
    if(size_write == -1){
        perror("Write in move_from_tar_to_tar");
        return -1;
    }

    close(fd_source);
    close(fd_target);
    free(buf);


    return 0;

}

int mv_from_ext_to_tar(){

    return 0;

}

int mv_from_tar_to_ext(){

    return 0;

}