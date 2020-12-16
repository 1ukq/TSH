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
    if(check_sys_call(fd_source, "open in copy_from_tar") == -1) return -1;

    int shift = 0;

    size_read = read(fd_source, &p, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in copy_from_tar") == -1) return -1;

    while(strcmp(p.name, path_file_source)){

        if(p.name[0] == '\0'){
            return -2;
        }

        sscanf(p.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
        int ret_lseek = lseek(fd_source, shift * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in copy_from_tar") == -1) return -1;

        size_read = read(fd_source, &p, BLOCK_SIZE);
        if(check_sys_call(size_read, "read in copy_from_tar") == -1) return -1;

    }

    sscanf(p.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in copy_from_tar");
        return -1;
    }

    size_read = read(fd_source, buf, shift * BLOCK_SIZE);
    if(check_sys_call(size_read, "read in copy_from_tar") == -1) return -1;

    int nb_char_in_buf = 0;
    for(int i = 0; i < shift * BLOCK_SIZE; i++){
        if(buf[i] == '\0'){
            nb_char_in_buf = i;
            break;
        }
    }

    size_write = write(fd_dest, buf, nb_char_in_buf);
    if(check_sys_call(size_write, "write in copy_from_tar") == -1) return -1;

    close(fd_source);
    free(buf);

    return nb_char_in_buf;

}

int copy_in_tar(const char *path_file_source, const char *path_tar, const char *target){
    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in copy_in_tar") == -1) return -1;

    int fd_source = open(path_file_source, O_RDONLY);
    if(check_sys_call(fd_source, "open in copy_in_tar") == -1) return -1;

    int pos[3];
    int r = pos_file_in_tar(fd_tar, target, pos);
    if(r == -1) return -1;

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in copy_in_tar") == -1) return -1;

    char *buf = malloc(sizeof(char) * (size_tar - pos[1]));
    if(buf == NULL){
        perror("malloc in copy_in_tar");
        return -1;
    }
    memset(buf, '\0', size_tar - pos[1]);
    int ret_lseek = lseek(fd_tar, pos[1], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar") == -1) return -1;

    int rd = read(fd_tar, buf, size_tar - pos[1]);
    if(check_sys_call(rd, "read in copy_in_tar") == -1) return -1;

    int size_source = lseek(fd_source, 0, SEEK_END);
    if(check_sys_call(size_source, "lseek in copy_in_tar") == -1) return -1;

    int nb_blocks = size_source % BLOCK_SIZE == 0 ? size_source / BLOCK_SIZE : (size_source / BLOCK_SIZE) + 1;
    char *buf_source = malloc(nb_blocks * BLOCK_SIZE);
    if(buf_source == NULL){
        perror("malloc in copy_in_tar");
        return -1;
    }
    memset(buf_source, '\0', nb_blocks * BLOCK_SIZE);
    ret_lseek = lseek(fd_source, 0, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar") == -1) return -1;

    rd = read(fd_source, buf_source, size_source);
    if(check_sys_call(rd, "read in copy_in_tar") == -1) return -1;

    ret_lseek = lseek(fd_tar, pos[0], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar") == -1) return -1;

    int wr = write(fd_tar, buf_source, nb_blocks * BLOCK_SIZE);
    if(check_sys_call(wr, "write in copy_in_tar") == -1) return -1;
    wr = write(fd_tar, buf, size_tar - pos[1]);
    if(check_sys_call(wr, "write in copy_in_tar") == -1) return -1;

    //change header
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar") == -1) return -1;

    struct posix_header header;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_in_tar") == -1) return -1;

    sprintf(header.size, "%011o", size_source);
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar") == -1) return -1;

    wr = write(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in copy_in_tar") == -1) return -1;

    free(buf);
    free(buf_source);
    close(fd_tar);
    close(fd_source);
    return 0;
}

int copy_from_tar_to_tar(const char *path_tar_src, const char* path_tar_dest, const char *file_src, const char *file_dest){
    int fd_src = open(path_tar_src, O_RDONLY);
    if(check_sys_call(fd_src, "open in copy_from_tar_to_tar") == -1) return -1;
    int fd_dest = open(path_tar_dest, O_RDWR);
    if(check_sys_call(fd_src, "open in copy_from_tar_to_tar") == -1) return -1;

    int pos_src[3];
    int pos_dest[3];

    int ret1 = pos_file_in_tar(fd_src, file_src, pos_src);
    if(ret1 == -1) return -1;
    int ret2 = pos_file_in_tar(fd_dest, file_dest, pos_dest);
    if(ret2 == -1) return -1;

    int size_src = pos_src[1] - pos_src[0];
    char *buf_src = malloc(size_src);
    if(buf_src == NULL){
        perror("malloc1 in copy_from_tar_to_tar");
        return -1;
    }

    int ret_lseek = lseek(fd_src, pos_src[0], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar") == -1) return -1;

    int rd = read(fd_src, buf_src, size_src);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar") == -1) return -1;

    int size_tar_dest = lseek(fd_dest, 0, SEEK_END);
    if(check_sys_call(size_tar_dest, "lseek in copy_from_tar_to_tar") == -1) return -1;
    char *buf_mv = malloc(size_tar_dest - pos_dest[1]);
    if(buf_mv == NULL){
        perror("malloc2 in copy_from_tar_to_tar");
        return -1;
    }
    ret_lseek = lseek(fd_dest, pos_dest[1], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar") == -1) return -1;
    rd = read(fd_dest, buf_mv, size_tar_dest - pos_dest[1]);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar") == -1) return -1;

    ret_lseek = lseek(fd_dest, pos_dest[0], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar") == -1) return -1;
    int wr = write(fd_dest, buf_src, size_src);
    if(check_sys_call(wr, "write in copy_from_tar_to_tar") == -1) return -1;
    wr = write(fd_dest, buf_mv, size_tar_dest - pos_dest[1]);
    if(check_sys_call(wr, "write in copy_from_tar_to_tar") == -1) return -1;

    //change header
    ret_lseek = lseek(fd_dest, pos_dest[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar") == -1) return -1;

    struct posix_header header;
    rd = read(fd_dest, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar") == -1) return -1;

    sprintf(header.size, "%011o", pos_src[2] - pos_src[0]);
    set_checksum(&header);
    ret_lseek = lseek(fd_dest, pos_dest[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar") == -1) return -1;

    wr = write(fd_dest, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in copy_from_tar_to_tar") == -1) return -1;

    close(fd_src);
    close(fd_dest);
    free(buf_src);
    free(buf_mv);

    return 0;
}

int copy_from_tar_r(const char *path_tar_source, const char *path_dir, const char *path_file_dest){
    int fd_tar = open(path_tar_source, O_RDONLY);
    if(check_sys_call(fd_tar, "open in copy_from_tar_r") == -1) return -1;

    int fd_dest = open(path_file_dest, O_WRONLY|O_APPEND);
    if(check_sys_call(fd_dest, "open in copy_from_tar_r") == -1) return -1;

    struct posix_header header;
    int rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_from_tar_r") == -1) return -1;

    int ret_lseek = 0;
    int nb_blocks = 0;
    int file_size = 0;
    int wr = 0;

    char *buf;

    while(header.name[0] != '\0'){

        sscanf(header.size, "%o", &file_size);
        nb_blocks = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
        if(strstr(header.name, path_dir) != NULL){
            buf = malloc(file_size);
            if(buf == NULL){
                perror("malloc in copy_from_tar_r");
                return -1;
            }
            rd = read(fd_tar, buf, file_size);
            if(check_sys_call(rd, "read in copy_from_tar_r") == -1) return -1;
            wr = write(fd_dest, buf, file_size);
            if(check_sys_call(wr, "write in copy_from_tar_r") == -1) return -1;
            ret_lseek = lseek(fd_tar, -file_size, SEEK_CUR);
            if(check_sys_call(ret_lseek, "lseek in copy_from_tar_r") == -1) return -1;
        }
        ret_lseek = lseek(fd_tar, nb_blocks * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in copy_from_tar_r") == -1) return -1;
        rd = read(fd_tar, &header, BLOCK_SIZE);
        if(check_sys_call(rd, "read in copy_from_tar_r") == -1) return -1;

    }
    close(fd_tar);
    close(fd_dest);
    free(buf);
    return 0;
}

int copy_in_tar_r(const char *path_tar, const char *file_dest, char *path_dir){
    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in copy_in_tar_r") == -1) return -1;

    DIR *dir;
    dir = opendir(path_dir);
    if(dir == NULL){
        printf("No such directory");
        return -1;
    }

    struct dirent *entry;
    entry = readdir(dir);

    int fd = 0;
    int st = 0;
    int rd = 0;
    int wr = 0;
    int size_copied = 0;
    int ret_lseek = 0;
    char *entry_name;
    char *p;
    struct stat statbuf;
    char *buf;
    int pos[3];
    if(pos_file_in_tar(fd_tar, file_dest, pos) == -1) return -1;
    int pos_s = pos[0];

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in copy_in_tar_r") == -1) return -1;
    char *buf_mv = malloc(size_tar - pos[1]);
    if(buf_mv == NULL){
        perror("malloc in copy_in_tar_r");
        return -1;
    }
    ret_lseek = lseek(fd_tar, pos[1], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar_r") == -1) return -1;
    rd = read(fd_tar, buf_mv, size_tar - pos[1]);
    if(check_sys_call(rd, "read in copy_in_tar_r") == -1) return -1;
    

    while(entry != NULL){

        entry_name = entry -> d_name;
        if(!strcmp(entry_name, ".") || !strcmp(entry_name, "..")){
            entry = readdir(dir);
            continue;
        }
        p = concatenate(path_dir, entry_name);
        st = stat(p, &statbuf);
        if(check_sys_call(st, "stat in copy_in_tar_r") == -1) return -1;

        if(S_ISREG(statbuf.st_mode)){
            fd = open(p, O_RDONLY);
            if(check_sys_call(fd, "open in copy_in_tar_r") == -1) return -1;
            buf = malloc(statbuf.st_size);
            if(buf == NULL){
                perror("malloc in copy_in_tar_r");
                return -1;
            }
            rd = read(fd, buf, statbuf.st_size);
            if(check_sys_call(rd, "read in copy_in_tar_r") == -1) return -1;
            ret_lseek = lseek(fd_tar, pos_s, SEEK_SET);
            if(check_sys_call(ret_lseek, "lseek in copy_in_tar_r") == -1) return -1;
            wr = write(fd_tar, buf, statbuf.st_size);
            if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;
            pos_s += statbuf.st_size;
            size_copied += statbuf.st_size;
        }

        entry = readdir(dir);

    }

    int nb_blocks = size_copied % BLOCK_SIZE == 0 ? size_copied / BLOCK_SIZE : (size_copied / BLOCK_SIZE) + 1;
    int s = (nb_blocks * BLOCK_SIZE) - size_copied;
    char *complete = malloc(s);
    if(complete == NULL){
        perror("malloc in copy_in_tar_r");
        return -1;
    }
    memset(complete, '\0', s);
    write(fd_tar, complete, s);
    if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;
    write(fd_tar, buf_mv, size_tar - pos[1]);
    if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;

    //change header
    struct posix_header header;
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar_r") == -1) return -1;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_in_tar_r") == -1) return -1;
    sprintf(header.size, "%011o", size_copied);
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_in_tar_r") == -1) return -1;
    wr = write(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;

    close(fd_tar);
    closedir(dir);
    free(complete);
    free(buf);
    free(buf_mv);
    return 0;
}

int copy_from_tar_to_tar_r(const char *path_tar_src, const char *path_tar_dest, const char *src, const char *dest){
    int fd_src = open(path_tar_src, O_RDONLY);
    if(check_sys_call(fd_src, "open in copy_from_tar_to_tar_r") == -1) return -1;
    int fd_dest = open(path_tar_dest, O_RDWR);
    if(check_sys_call(fd_src, "open in copy_from_tar_to_tar_r") == -1) return -1;

    int pos[3];
    int ret = pos_file_in_tar(fd_dest, dest, pos);
    if(ret == -1) return -1;
    int size_tar_dest = lseek(fd_dest, 0, SEEK_END);
    if(check_sys_call(size_tar_dest, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
    char *buf_mv = malloc(size_tar_dest - pos[1]);
    if(buf_mv == NULL){
        perror("malloc in copy_from_tar_to_tar_r");
        return -1;
    }
    int ret_lseek = lseek(fd_dest, pos[1], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
    int rd = read(fd_dest, buf_mv, size_tar_dest - pos[1]);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar_r") == -1) return -1;

    struct posix_header header;
    rd = read(fd_src, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar_r") == -1) return -1;

    int pos_s = pos[0];
    int nb_blocks = 0;
    int file_size = 0;
    int wr = 0;
    int size_copied = 0;

    char *buf;

    while(header.name[0] != '\0'){

        sscanf(header.size, "%o", &file_size);
        nb_blocks = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;

        if(strstr(header.name, src) != NULL){

            buf = malloc(file_size);
            if(buf == NULL){
                perror("malloc in copy_from_tar_to_tar_r");
                return -1;
            }
            rd = read(fd_src, buf, file_size);
            if(check_sys_call(rd, "read in copy_from_tar_to_tar_r") == -1) return -1;
            ret_lseek = lseek(fd_dest, pos_s, SEEK_SET);
            if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
            wr = write(fd_dest, buf, file_size);
            if(check_sys_call(wr, "write in copy_from_tar_to_tar_r") == -1) return -1;
            pos_s += file_size;
            size_copied += file_size;
            ret_lseek = lseek(fd_src, -file_size, SEEK_CUR);
            if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;

        }

        ret_lseek = lseek(fd_src, nb_blocks * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
        rd = read(fd_src, &header, BLOCK_SIZE);
        if(check_sys_call(rd, "read in copy_from_tar_to_tar_r") == -1) return -1;

    }
    
    nb_blocks = size_copied % BLOCK_SIZE == 0 ? size_copied / BLOCK_SIZE : (size_copied / BLOCK_SIZE) + 1;
    int s = (nb_blocks * BLOCK_SIZE) - size_copied;
    char *complete = malloc(s);
    if(complete == NULL){
        perror("malloc in copy_in_tar_r");
        return -1;
    }
    memset(complete, '\0', s);
    write(fd_dest, complete, s);
    if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;
    write(fd_dest, buf_mv, size_tar_dest - pos[1]);
    if(check_sys_call(wr, "write in copy_in_tar_r") == -1) return -1;


    //change header
    ret_lseek = lseek(fd_dest, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
    rd = read(fd_dest, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in copy_from_tar_to_tar_r") == -1) return -1;
    sprintf(header.size, "%011o", size_copied);
    set_checksum(&header);
    ret_lseek = lseek(fd_dest, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in copy_from_tar_to_tar_r") == -1) return -1;
    wr = write(fd_dest, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in copy_from_tar_to_tar_r") == -1) return -1;


    close(fd_src);
    close(fd_dest);

    return 0;

}

void cat(const char *path_tar, const char *path_file_source){
    int n = copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    //return n;

}
