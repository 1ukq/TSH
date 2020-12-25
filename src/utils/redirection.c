#include "redirection.h"

// Permet de mettre dans un buffer ce qu'il y a dans un pipe

int buffarize_output(int fd_read_end, char *buffer){
    int rd;
    int nb_loop = 0;
    int nb_blocks = 2;
    char buf[BLOCK_SIZE];
    memset(buf, '\0', BLOCK_SIZE);
    while((rd = read(fd_read_end, buf, BLOCK_SIZE)) > 0){
        memcpy(buffer + (nb_loop * BLOCK_SIZE), buf, BLOCK_SIZE);
        memset(buf, '\0', BLOCK_SIZE);
        buffer = realloc(buffer, nb_blocks * BLOCK_SIZE);
        nb_loop++;
        nb_blocks++;
    }
    return nb_loop * BLOCK_SIZE;
}

// Permet d'écrire dans un pipe le contenu d'un input (input = contenu d'un fichier dans un tar)
int input_to_pipe(int fd_write_end, char *path_tar, const char *path_in_tar){
    int fd_tar = open(path_tar, O_RDONLY);
    if(check_sys_call(fd_tar, "open in input_to_pipe") == -1) return -1;
    int pos[3];
    pos_file_in_tar(fd_tar, path_in_tar, pos);
    char *buf = malloc(sizeof(char) * (pos[2] - pos[0]));

    int ret_lseek = lseek(fd_tar, pos[0], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in input_to_pipe") == -1) return -1;

    int rd = read(fd_tar, buf, pos[2] - pos[0]);
    if(check_sys_call(rd, "read in input_to_pipe") == -1) return -1;

    int wr = write(fd_write_end, buf, pos[2] - pos[0]);
    if(check_sys_call(wr, "write in input_to_pipe") == -1) return -1;
    return 0;
}

// Permet de rediriger l'entrée standard dans l'entrée de lecteur d'un pipe (qui contient le contenu d'un fichier dans un tar)
int red_input_in_tar(int fd_read_end){
    int ret_dup2 = dup2(fd_read_end, STDIN_FILENO);
    if(check_sys_call(ret_dup2, "dup2 in red_input_in_tar") == -1) return -1;
    return 0;
}

// Permet d'écire dans un fichier d'un tar un buffer qui contient l'output ou l'erreur d'une commande. Ecrase le contenu du fichier
int red_trunc_in_tar(const char *path_tar, const char *path_in_tar, char *buf_cmd, int size_buf_cmd){
    int pos[3];
    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in red_trunc_in_tar") == -1) return -1;

    int ret = pos_file_in_tar(fd_tar, path_in_tar, pos);
    if(ret == -1){
        return -1;
    }

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in red_trunc_in_tar") == -1) return -1;

    int ret_lseek = lseek(fd_tar, pos[1], SEEK_SET); // end of file (path_in_tar file)
    if(check_sys_call(ret_lseek, "lseek in red_trunc_in_tar") == -1) return -1;

    char *buf = malloc(sizeof(char) * (size_tar - ret_lseek));
    memset(buf, '\0', sizeof(char) * (size_tar - ret_lseek));
    int rd = read(fd_tar, buf, sizeof(char) * (size_tar - ret_lseek));
    if(check_sys_call(rd, "read in red_trunc_in_tar") == -1) return -1;

    ret_lseek = lseek(fd_tar, pos[0], SEEK_SET); // beginning of file (path_in_tar file)
    if(check_sys_call(ret_lseek, "lseek in red_trunc_in_tar") == -1) return -1;

    int wr = write(fd_tar, buf_cmd, sizeof(char) * size_buf_cmd);
    if(check_sys_call(wr, "write in red_trunc_in_tar") == -1) return -1;

    wr = write(fd_tar, buf, sizeof(char) * (size_tar - pos[1]));
    if(check_sys_call(wr, "write in red_trunc_in_tar") == -1) return -1;

    // changing file header
    int size_file = str_length(buf_cmd);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in red_trunc_in_tar") == -1) return -1;
    struct posix_header header;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in red_trunc_in_tar") == -1) return -1;
    //sprintf(header.size, "%011o", size_file); //Line MacOS
    sprintf(header.size, "%011o", size_file);
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in red_trunc_in_tar") == -1) return -1;
    wr = write(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in red_trunc_in_tar") == -1) return -1;

    free(buf);
    close(fd_tar);

    return 0;
}

// Permet d'écire dans un fichier d'un tar un buffer qui contient l'output ou l'erreur d'une commande. Ecris à la suite du fichier
int red_append_in_tar(const char *path_tar, const char *path_in_tar, char *buf_cmd, int size_buf_cmd){
    int pos[3];
    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in red_append_in_tar") == -1) return -1;

    int ret = pos_file_in_tar(fd_tar, path_in_tar, pos);
    if(ret == -1){
        return -1;
    }

    int size_file = pos[2] - pos[0];
    int size = str_length(buf_cmd);
    int nb_blocks = (size + size_file) % BLOCK_SIZE == 0 ? (size + size_file) / BLOCK_SIZE : ((size + size_file) / BLOCK_SIZE) + 1;

    // buffer_wr
    char *buffer_wr = malloc(sizeof(char) * nb_blocks * BLOCK_SIZE);
    memset(buffer_wr, '\0', sizeof(char) * nb_blocks * BLOCK_SIZE);
    int begin_file = lseek(fd_tar, pos[0], SEEK_SET);
    if(check_sys_call(begin_file, "lseek in red_append_in_tar") == -1) return -1;
    int rd = read(fd_tar, buffer_wr, size_file);
    if(check_sys_call(rd, "read in red_append_in_tar") == -1) return -1;
    memcpy(buffer_wr + size_file, buf_cmd, size);

    // buffer_mv
    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in red_append_in_tar") == -1) return -1;
    int end_file = lseek(fd_tar, pos[1], SEEK_SET);
    if(check_sys_call(end_file, "lseek in red_append_in_tar") == -1) return -1;
    char *buffer_mv = malloc(sizeof(char) * (size_tar - end_file));
    memset(buffer_mv, '\0', sizeof(char) * (size_tar - end_file));
    rd = read(fd_tar, buffer_mv, (size_tar - end_file));
    if(check_sys_call(rd, "read in red_append_in_tar") == -1) return -1;

    int ret_lseek = lseek(fd_tar, pos[0], SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in red_append_in_tar") == -1) return -1;
    int wr = write(fd_tar, buffer_wr, nb_blocks * BLOCK_SIZE);
    if(check_sys_call(wr, "write in red_append_in_tar") == -1) return -1;
    wr = write(fd_tar, buffer_mv, (size_tar - end_file));
    if(check_sys_call(wr, "write in red_append_in_tar") == -1) return -1;

    // change header file
    struct posix_header header;
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in red_append_in_tar") == -1) return -1;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(rd, "read in red_append_in_tar") == -1) return -1;
    //sprintf(header.size, "%011o", (size + size_file)); //Line MacOS
    sprintf(header.size, "%011o", (size + size_file));
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in red_append_in_tar") == -1) return -1;
    wr = write(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in red_append_in_tar") == -1) return -1;

    free(buffer_wr);
    free(buffer_mv);
    close(fd_tar);

    return 0;
}



//Les fonctions suivantes servent aux redirection qui n'impliquent pas de tar

int red_input_out_tar(const char *file){
    int fd = open(file, O_RDONLY);
    if(check_sys_call(fd, "open in red_input_out_tar") == -1) return -1;
    
    int ret_dup2 = dup2(fd, STDIN_FILENO);
    if(check_sys_call(ret_dup2, "dup2 in red_input_out_tar") == -1){
        close(fd);
        return -1;
    }
    return fd;
}

int red_output_append_out_tar(const char *file){
    int fd = open(file, O_WRONLY|O_APPEND);
    if(check_sys_call(fd, "open in red_output_append_out_tar") == -1) return -1;

    int ret_dup2 = dup2(fd, STDOUT_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_output_append_out_tar");
        return -1;
    }
    return fd;
}

int red_output_trunc_out_tar(const char *file){
    int fd = open(file, O_WRONLY|O_TRUNC);
    if(check_sys_call(fd, "open in red_output_trunc_out_tar") == -1) return -1;

    int ret_dup2 = dup2(fd, STDOUT_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_output_trunc_out_tar");
        return -1;
    }
    return fd;
}

int red_err_append_out_tar(const char *file){
    int fd = open(file, O_WRONLY|O_APPEND);
    if(check_sys_call(fd, "open in red_err_append_out_tar") == -1) return -1;

    int ret_dup2 = dup2(fd, STDERR_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_err_append_out_tar");
        return -1;
    }
    return fd;
}

int red_err_trunc_out_tar(const char *file){
    int fd = open(file, O_WRONLY|O_TRUNC);
    if(check_sys_call(fd, "open in red_err_trunc_out_tar") == -1) return -1;

    int ret_dup2 = dup2(fd, STDERR_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_err_trunc_out_tar");
        return -1;
    }
    return fd;
}