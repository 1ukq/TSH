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
    if(check_sys_call(fd_source, "open in copy_from_tar") == -1) return -2;

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

/*
 * path_tar: chemin absolu vers l'archive
 * path_dit_src: chemin absolu du répertoire source depuis l'archive
 * path_dir_dest: chemin absolu du répertoire de destination
*/
int copy_from_tar_to_dir_r(const char *path_tar, const char *path_dir_src, const char *path_dir_dest){}

int cat(const char *path_tar, const char *path_file_source){
    int n = copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    return n;

}
