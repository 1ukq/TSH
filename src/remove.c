#include "remove.h"


int remove_file_from_tar(const char *path_tar, const char *path_file_source){

    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in remove_file_from_tar");
        return -1;
    }

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(size_tar == -1){
        perror("lseek in remove_file_from_tar");
        return -1;
    }
    int pos = lseek(fd_tar, 0, SEEK_SET);
    if(pos == -1){
        perror("lseek in remove_file_from_tar");
        return -1;
    }

    struct posix_header header;
    ssize_t size_read = read(fd_tar, &header, BLOCK_SIZE);
    if(size_read == -1){
        perror("read in remove_file_from_tar");
        return -1;
    }

    int file_size = 0;
    int shift = 0;

    while(strcmp(header.name, path_file_source)){

        sscanf(header.size, "%o", &file_size);
        shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
        pos = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
        if(pos == -1){
            perror("lseek in remove_file_from_tar");
            return -1;
        }

        size_read = read(fd_tar, &header, BLOCK_SIZE);
        if(size_read == -1){
            perror("read in remove_file_from_tar");
            return -1;
        }
    }

    sscanf(header.size, "%o", &file_size);
    shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;

    pos = lseek(fd_tar, -BLOCK_SIZE, SEEK_CUR);
    if(pos == -1){
        perror("lseek in remove_file_from_tar");
        return -1;
    }

    char *buf_write = malloc(sizeof(char) * (size_tar - pos));
    memset(buf_write, '\0', sizeof(char) * (size_tar - pos));

    int n = pos;

    pos = lseek(fd_tar, BLOCK_SIZE * (shift + 1), SEEK_CUR);
    if(pos == -1){
        perror("lseek in remove_file_from_tar");
        return -1;
    }
    
    size_read = read(fd_tar, buf_write, (size_tar - n));
    if(size_read == -1){
        perror("read in remove_file_from_tar");
        return -1;        
    }


    pos = lseek(fd_tar, n, SEEK_SET);
    if(pos == -1){
        perror("lseek in remove_file_from_tar");
        return -1;        
    }

    int size_write = write(fd_tar, buf_write, size_tar - n);
    if(size_write == -1){
        perror("write in remove_file_from_tar");
        return -1;        
    }

    close(fd_tar);
    free(buf_write);

    return 0;
    
}