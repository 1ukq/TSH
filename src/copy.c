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

int copy_in_tar(const char *path_file_source, const char *path_tar, const char *target){
    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in copy_in_tar");
        return -1;
    }
    int fd_source = open(path_file_source, O_RDONLY);
    int pos[3];
    int r = pos_file_in_tar(fd_tar, target, pos);
    if(r == -1) return -1;
    int size_tar = lseek(fd_tar, 0, SEEK_END);

    char *buf = malloc(sizeof(char) * (size_tar - pos[1]));
    memset(buf, '\0', size_tar - pos[1]);
    int ret_lseek = lseek(fd_tar, pos[1], SEEK_SET);
    int rd = read(fd_tar, buf, size_tar - pos[1]);
    printf("pos 1 = %d\n", pos[1]);

    int size_source = lseek(fd_source, 0, SEEK_END);
    int nb_blocks = size_source % BLOCK_SIZE == 0 ? size_source / BLOCK_SIZE : (size_source / BLOCK_SIZE) + 1;
    char *buf_source = malloc(nb_blocks * BLOCK_SIZE);
    memset(buf_source, '\0', nb_blocks * BLOCK_SIZE);
    ret_lseek = lseek(fd_source, 0, SEEK_SET);
    rd = read(fd_source, buf_source, size_source);

    ret_lseek = lseek(fd_tar, pos[0], SEEK_SET);
    int wr = write(fd_tar, buf_source, size_source);
    wr = write(fd_tar, buf, size_tar - pos[1]);

    //change header
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    struct posix_header header;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    sprintf(header.size, "%011o", size_source);
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    wr = write(fd_tar, &header, BLOCK_SIZE);

    free(buf);
    free(buf_source);
    close(fd_tar);
    close(fd_source);
    return 0;
}

void cat(const char *path_tar, const char *path_file_source){

    int n = copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    if(n == 1) perror("");

}

