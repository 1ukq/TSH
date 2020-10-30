#include "insert.h"

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes){

    for(int i = 0; i < nbytes; i++) if(buf1[i] != buf2[i]) return 0;
    return 1;

}

void copy_string(char *str1, char *str2, int length){

    for(int i = 0; i < length; i++){
        str1[i] = str2[i];
    }

}

int find_next_block(int fd_tar, struct stat *restrict buf_stat){

    int nb_blocks = 0;
    int size_tar = buf_stat -> st_size;
    int pos = 0;

    char *content = malloc(sizeof(char) * size_tar);
    char *buf1 = malloc(sizeof(char) * BLOCK_SIZE);
    char *buf2 = malloc(sizeof(char) * BLOCK_SIZE);
    memset(buf2, '\0', BLOCK_SIZE);

    int n = read(fd_tar, content, size_tar);
    if(n == -1){
        perror("read in find_next_block");
        return -1;
    }

    copy_string(buf1, content, BLOCK_SIZE);

    while(!compare_buffers_of_same_size(buf1, buf2, BLOCK_SIZE)){

        pos += BLOCK_SIZE;
        copy_string(buf1, content + pos, BLOCK_SIZE);
        nb_blocks++;

    }

    return nb_blocks;

}

char *buffarize(const char *restrict path_file_source, struct stat *restrict buf){

    int fd_source = open(path_file_source, O_RDONLY);
    if(fd_source == -1){
        perror("open in buffarize");
        return NULL;
    }

    off_t size = buf -> st_size;

    int nb_blocks = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    char *file_source_buf = malloc(sizeof(char) * BLOCK_SIZE * nb_blocks);
    if(file_source_buf == NULL){
        perror("malloc in buffarize");
        return NULL;
    }

    int nb_char_read = read(fd_source, file_source_buf, size);
    if(nb_char_read == -1){
        perror("read in buffarize");
        return NULL;
    }

    return file_source_buf;

}

int insert_file_in_tar(const char *path_tar, const char *path_file_source){

    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in insert_file_in_tar");
        return -1;
    }

    //int lseek_ret = lseek(fd_tar, - 2 * BLOCK_SIZE, SEEK_END); //This line is for bsdtar not GNU tar (bsdtar is the default tar utility on MacOS)
    struct stat stat_tar;
    stat(path_tar, &stat_tar);
    int b = find_next_block(fd_tar, &stat_tar);
    int lseek_ret = lseek(fd_tar, b * BLOCK_SIZE, SEEK_SET);
    if(lseek_ret == -1){
        perror("lseek in insert_file_in_tar");
        return -1;
    }

    struct stat stat_buf_source;
    int check_stat = stat(path_file_source, &stat_buf_source);
    if(check_stat == -1){
        perror("stat in insert_file_in_tar");
        return -1;
    }
    struct posix_header header;
    memset(&header, '\0', sizeof(char) * BLOCK_SIZE);

    create_file_header(path_file_source, &stat_buf_source, &header);
    char *file_source_buf = buffarize(path_file_source, &stat_buf_source);

    int wr = write(fd_tar, &header, BLOCK_SIZE);
    if(wr == -1){
        perror("write in insert_file_in_tar");
        return -1;
    }

    int size = stat_buf_source.st_size;
    int nb_blocks = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    wr = write(fd_tar, file_source_buf, nb_blocks * BLOCK_SIZE);
    if(wr == -1){
        perror("write in insert_file_in_tar");
        return -1;
    }

    free(file_source_buf);
    close(fd_tar);

    return 0;

}