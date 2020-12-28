#include "remove.h"


int remove_file_from_tar(const char *path_tar, const char *path_file_source){

    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in remove_file_from_tar") == -1) return -2;

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in remove_file_from_tar") == -1) return -1;

    int pos = lseek(fd_tar, 0, SEEK_SET);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    struct posix_header header;
    ssize_t size_read = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;

    int file_size = 0;
    int shift = 0;

    while(strcmp(header.name, path_file_source)){

        if(header.name[0] == '\0'){
            return -2;
        }

        sscanf(header.size, "%o", &file_size);
        shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
        pos = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

        size_read = read(fd_tar, &header, BLOCK_SIZE);
        if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;
    }

    if(header.typeflag == DIRTYPE){
        printf("Directory\n");
        return -1;
    }

    sscanf(header.size, "%o", &file_size);
    shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;

    pos = lseek(fd_tar, -BLOCK_SIZE, SEEK_CUR);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    char *buf_write = malloc(sizeof(char) * (size_tar - pos));
    memset(buf_write, '\0', sizeof(char) * (size_tar - pos));

    int n = pos;

    pos = lseek(fd_tar, BLOCK_SIZE * (shift + 1), SEEK_CUR);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    size_read = read(fd_tar, buf_write, (size_tar - n));
    if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;


    pos = lseek(fd_tar, n, SEEK_SET);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    int size_write = write(fd_tar, buf_write, size_tar - n);
    if(check_sys_call(size_write, "write in remove_file_from_tar") == -1) return -1;

    close(fd_tar);
    free(buf_write);

    return 0;

}

int remove_file_from_tar_r(const char *path_tar, const char *path_dir){

    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in remove_file_from_tar_r") == -1) return -2;

    struct posix_header header;
    int rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in remove_file_from_tar_r") == -1) return -1;

    int ret_lseek = 0;

    int size_tar = 0;
    int pos_from = 0;
    int pos_to = 0;
    int nb_blocks = 0;
    int file_size = 0;

    while(header.name[0] != '\0'){

        if(!strcmp(header.name, path_dir) && header.typeflag != DIRTYPE){
            printf("Not a directory\n");
            return -1;
        }

        if(strstr(header.name, path_dir) != NULL){
            if(header.typeflag == DIRTYPE){
                pos_from = lseek(fd_tar, -BLOCK_SIZE, SEEK_CUR);
                pos_to = lseek(fd_tar, BLOCK_SIZE, SEEK_CUR);
                size_tar = lseek(fd_tar, 0, SEEK_END);
                int s = suppress_file(fd_tar, pos_from, pos_to, size_tar);
                if(s == -1) return -1;
                ret_lseek = lseek(fd_tar, 0, SEEK_SET);
                if(check_sys_call(ret_lseek, "lseek in remove_file_from_tar_r") == -1) return -1;
                rd = read(fd_tar, &header, BLOCK_SIZE);
                if(check_sys_call(rd, "read in remove_file_from_tar_r") == -1) return -1;
                continue;
            }
            pos_from = lseek(fd_tar, -BLOCK_SIZE, SEEK_CUR);
            if(check_sys_call(pos_from, "lseek in remove_file_from_tar_r") == -1) return -1;
            size_tar = lseek(fd_tar, 0, SEEK_END);
            if(check_sys_call(size_tar, "lseek in remove_file_from_tar_r") == -1) return -1;
            sscanf(header.size, "%o", &file_size);
            nb_blocks = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
            int pos_to = pos_from + ((nb_blocks + 1) * BLOCK_SIZE);
            int s = suppress_file(fd_tar, pos_from, pos_to, size_tar);
            if(s == -1) return -1;
            ret_lseek = lseek(fd_tar, 0, SEEK_SET);
            if(check_sys_call(ret_lseek, "lseek in remove_file_from_tar_r") == -1) return -1;
            rd = read(fd_tar, &header, BLOCK_SIZE);
            if(check_sys_call(rd, "read in remove_file_from_tar_r") == -1) return -1;
        }

        sscanf(header.size, "%o", &file_size);
        nb_blocks = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
        ret_lseek = lseek(fd_tar, nb_blocks * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in remove_file_from_tar_r") == -1) return -1;
        rd = read(fd_tar, &header, BLOCK_SIZE);
        if(check_sys_call(rd, "read in remove_file_from_tar_r") == -1) return -1;

    }
    close(fd_tar);
    return 0;

}
