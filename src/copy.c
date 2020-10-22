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

    while(strcmp(p.name, path_file_source)){

        size_read = read(fd_source, &p, BLOCK_SIZE);
        if(size_read == -1){
            perror("read in copy_from_tar");
            return -1;
        }

        sscanf(p.size, "%x", &size);

        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    }

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

}

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes){

    for(int i = 0; i < nbytes; i++) if(buf1[i] != buf2[i]) return 0;
    return 1;

}

int find_next_block(int fd_tar){

    int nb_blocks = 0;

    char *buf1 = malloc(sizeof(char) * BLOCK_SIZE);
    char *buf2 = malloc(sizeof(char) * BLOCK_SIZE);
    memset(buf2, '\0', BLOCK_SIZE);

    int n = read(fd_tar, buf1, BLOCK_SIZE);
    if(n == -1){
        perror("read in find_next_block");
        return -1;
    }

    while(!compare_buffers_of_same_size(buf1, buf2, BLOCK_SIZE)){
        read(fd_tar, buf1, BLOCK_SIZE);
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

void init_type_file(struct stat *restrict buf, struct posix_header *header){
    if(S_ISREG(buf -> st_mode)) (*header).typeflag = REGTYPE;
    else if(S_ISDIR(buf -> st_mode)) (*header).typeflag = DIRTYPE;
    else if(S_ISFIFO(buf -> st_mode)) (*header).typeflag = FIFOTYPE;
    else if(S_ISLNK(buf -> st_mode)) (*header).typeflag = LNKTYPE;
    else if(S_ISCHR(buf -> st_mode)) (*header).typeflag = CHRTYPE;
    else if(S_ISBLK(buf -> st_mode)) (*header).typeflag = BLKTYPE;
    else if(S_ISSOCK(buf -> st_mode)) (*header).typeflag = SOCKTYPE;
}

void init_mode_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> mode, "%s", "0000000\0");
    int usr = 0;
    int grp = 0;
    int oth = 0;
    if(buf -> st_mode & S_IRUSR) usr += 4;
    if(buf -> st_mode & S_IWUSR) usr += 2;
    if(buf -> st_mode & S_IXUSR) usr += 1;
    if(buf -> st_mode & S_IRGRP) grp += 4;
    if(buf -> st_mode & S_IWGRP) grp += 2;
    if(buf -> st_mode & S_IXGRP) grp += 1;
    if(buf -> st_mode & S_IROTH) oth += 4;
    if(buf -> st_mode & S_IWOTH) oth += 2;
    if(buf -> st_mode & S_IXOTH) oth += 1;
    (*header).mode[6] += oth;
    (*header).mode[5] += grp;
    (*header).mode[4] += usr;

}

void init_size_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> size, "%011llo", buf -> st_size);

}

void init_uid_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> uid, "%7o", buf -> st_uid);
    for(int i = 0; i < 8; i++) if((*header).uid[i] == ' ') (*header).uid[i] = '0';

}

void init_gid_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> gid, "%7o", buf -> st_gid);
    for(int i = 0; i < 8; i++) if((*header).gid[i] == ' ') (*header).gid[i] = '0';

}

void init_mtime_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> mtime, "%lo", buf -> st_mtimespec.tv_sec);

}

void init_name_file(struct posix_header *header, const char *path_file_source){

    sprintf(header -> name, "%s", path_file_source);

}

void init_magic_file(struct posix_header *header){

    sprintf(header -> magic, "%s", TMAGIC);

}

void init_version_file(struct posix_header *header){

    (*header).version[0] = ' ';
    (*header).version[1] = ' '; 

}

int create_file_header(const char *path_file_source, struct stat *restrict buf, struct posix_header *header){

    init_name_file(header, path_file_source);
    init_mode_file(buf, header);
    init_size_file(buf, header);
    init_type_file(buf, header);
    init_magic_file(header);
    init_version_file(header);
    init_uid_file(buf, header);
    init_gid_file(buf, header);
    init_mtime_file(buf, header);
    set_checksum(header);
    int n = check_checksum(header);
    if(!n){
        return -1;
    }
    return 0;

}

int insert_file_in_tar(const char *path_tar, const char *path_file_source){

    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in insert_file_in_tar");
        return -1;
    }

    //int lseek_ret = lseek(fd_tar, - 2 * BLOCK_SIZE, SEEK_END); //This line is for bsdtar not GNU tar (bsdtar is the default tar utility on MacOS)
    int b = find_next_block(fd_tar);
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