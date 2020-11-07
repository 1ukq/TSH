#include "move.h"

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

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, const char *path_in_tar){

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

int mv_from_dir_to_tar(const char *path_tar, const char *path_file_source, const char *path_in_tar){

    int ret = fork();
    if(ret == 0) execlp("rm", "rm", path_file_source, NULL);
    else if(ret == -1) perror("fork in mv_from_ext_to_tar");
    else insert_file_in_tar(path_tar, path_file_source);
    return 0;

}

int mv_from_tar_to_dir(const char *path_tar, const char *path_file_source, char *path_dest){

    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in mv_from_tar_to_dir");
        return -1;
    }

    int size = 0;
    int shift = 0;
    int ret_lseek = 0;

    struct posix_header header;
    int size_read = read(fd_tar, &header, BLOCK_SIZE);
    if(size_read == -1){
        perror("read in mv_from_tar_to_dir");
        return -1;
    }

    while(strcmp(header.name, path_file_source)){

        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

        ret_lseek = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
        if(ret_lseek == -1){
            perror("lseek in mv_from_tar_to_tar");
            return -1;
        }

        size_read = read(fd_tar, &header, BLOCK_SIZE);
        if(size_read == -1){
            perror("read in mv_from_tar_to_tar");
            return -1;
        }

    }

    sscanf(header.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in mv_from_tar_to_dir");
    }
    size_read = read(fd_tar, buf, sizeof(char) * shift * BLOCK_SIZE);
    if(size_read == -1){
        perror("read in mv_from_tar_to_dir");
        return -1;
    }

    ret_lseek = lseek(fd_tar, -BLOCK_SIZE * (shift + 1), SEEK_CUR);
    if(ret_lseek == -1){
        perror("lseek in mv_from_tar_to_dir");
        return -1;
    }

    int pos_from = ret_lseek;
    int pos_to = pos_from + (shift + 1) * BLOCK_SIZE;
    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(size_tar == -1){
        perror("lseek size_tar in mv_from_tar_to_dir");
        return -1;
    }
    int sup = suppress_file(fd_tar, pos_from, pos_to, size_tar);
    if(sup == -1) return -1;

    char *str = name(header.name);
    char *path = concatenate(path_dest, str);

    mode_t mode;
    sscanf(header.mode, "%o", &mode);

    int fd_dest = open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
    if(fd_dest == -1){
        perror("open dest in mv_from_tar_to_dir");
        return -1;
    }

    int size_write = write(fd_dest, buf, sizeof(char) * shift * BLOCK_SIZE);
    if(size_write == -1){
        perror("write in mv_from_tar_to_dir");
        return -1;
    }

    free(str);
    free(path);
    close(fd_tar);

    return 0;

}