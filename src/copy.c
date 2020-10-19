#include "copy.h"
#include "types/posix_header.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest){

    if(fd_dest == -1) perror("open");

    ssize_t size_read;
    ssize_t size_write;
    int size = 0;
    struct posix_header p;

    int fd_source = open(path_tar, O_RDONLY);
    if(fd_source == -1){
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

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
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
    free(buf);
    
    return nb_char_in_buf;

}

void cat(const char *path_tar, const char *path_file_source){

    int n = copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    if(n == -1) perror("");

}

char *buffarize(const char *restrict path_file_source, struct stat *restrict buf){

    int check_ret = stat(path_file_source, buf);
    if(check_ret == -1){
        perror("stat in buffarize");
        return NULL;
    }

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

void set_checksum(struct posix_header *hd) {
    memset(hd->chksum,' ',8);
    unsigned int sum = 0;
    char *p = (char *)hd;
    for (int i=0; i < BLOCK_SIZE; i++) { sum += p[i]; }
    sprintf(hd->chksum,"%06o",sum);
}

int check_checksum(struct posix_header *hd) {
    unsigned int checksum;
    sscanf(hd->chksum,"%o ", &checksum);
    unsigned int sum = 0;
    char *p = (char *)hd;
    for (int i=0; i < BLOCK_SIZE; i++) { sum += p[i]; }
    for (int i=0;i<8;i++) { sum += ' ' - hd->chksum[i]; }
    return (checksum == sum);
}

char file_type(struct stat *restrict buf){

    if(S_ISREG(buf -> st_mode)) return REGTYPE;
    else if(S_ISDIR(buf -> st_mode)) return DIRTYPE;
    else if(S_ISFIFO(buf -> st_mode)) return FIFOTYPE;
    else if(S_ISLNK(buf -> st_mode)) return LNKTYPE;
    else if(S_ISCHR(buf -> st_mode)) return CHRTYPE;
    else if(S_ISBLK(buf -> st_mode)) return BLKTYPE;
    else if(S_ISSOCK(buf -> st_mode)) return SOCKTYPE;
    else return 'R';

}

int file_mode(struct stat *restrict buf){

    int mode = 0000000;
    if(buf -> st_mode & S_IRUSR) mode += S_IRUSR;
    if(buf -> st_mode & S_IWUSR) mode += S_IWUSR;
    if(buf -> st_mode & S_IXUSR) mode += S_IXUSR;
    if(buf -> st_mode & S_IRGRP) mode += S_IRGRP;
    if(buf -> st_mode & S_IWGRP) mode += S_IWGRP;
    if(buf -> st_mode & S_IXGRP) mode += S_IXGRP;
    if(buf -> st_mode & S_IROTH) mode += S_IROTH;
    if(buf -> st_mode & S_IWOTH) mode += S_IWOTH;
    if(buf -> st_mode & S_IXOTH) mode += S_IXOTH;
    return mode;
    
}

void create_file_header(const char *restrict path_file_source, const char *file_name, struct stat *restrict buf, struct posix_header *header){

    int check_ret = stat(path_file_source, buf);
    if(check_ret == -1){
        perror("stat");
    }

    sprintf(header -> name, "%s", file_name);
    off_t file_size = buf -> st_size;
    sprintf(header -> size, "%011llo", file_size);
    char type = file_type(buf);
    header -> typeflag = type;
    set_checksum(header);
    (*header).version[0] = '0';
    (*header).version[1] = '0';
    sprintf(header -> magic, "%s", TMAGIC);
    int mode = file_mode(buf);
    sprintf(header -> mode, "%o", mode);

}

void insert_file_in_tar(const char *path_tar, const char *path_file_source, const char *file_name){

    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open");
    }

    int lseek_ret = lseek(fd_tar, - 2 * BLOCK_SIZE, SEEK_END);
    if(lseek_ret == -1){
        perror("lseek");
    }
    printf("LSEEK %d\n", lseek_ret);

    struct stat stat_buf_source;
    char *file_source_buf = buffarize(path_file_source, &stat_buf_source);

    struct posix_header header;
    struct stat buf;
    create_file_header(path_file_source, file_name, &buf, &header);

    int wr = write(fd_tar, &header, BLOCK_SIZE);

    int size = stat_buf_source.st_size;
    int nb_blocks = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    wr = write(fd_tar, file_source_buf, nb_blocks * BLOCK_SIZE);

}