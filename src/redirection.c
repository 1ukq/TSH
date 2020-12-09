#include "redirection.h"

int buffarize_output(int fd_read_end, char *buffer){
    int rd;
    int nb_loop = 0;
    char buf[BLOCK_SIZE];
    memset(buf, '\0', BLOCK_SIZE);
    while((rd = read(fd_read_end, buf, BLOCK_SIZE)) > 0){
        memcpy(buffer + (nb_loop * BLOCK_SIZE), buf, BLOCK_SIZE);
        memset(buf, '\0', BLOCK_SIZE);
        buffer = realloc(buffer, BLOCK_SIZE);
        nb_loop++;
    }
    return nb_loop * BLOCK_SIZE;
}

int red_output_to_pipe(int fd_write_end){
    int ret_dup2 = dup2(fd_write_end, STDOUT_FILENO);
    if(ret_dup2 == -1){
        perror("dup2 in red_output_to_pipe");
        return -1;
    }
    return 0;
}

int red_input_out_tar(const char *file){
    int fd = open(file, O_RDONLY);
    if(fd == -1){
        perror("open in red_input_out_tar");
        return -1;
    }
    int ret_dup2 = dup2(fd, STDIN_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_input");
        return -1;
    }
    return fd;
}

int red_output_append_out_tar(const char *file){
    int fd = open(file, O_WRONLY|O_APPEND);
    if(fd == -1){
        perror("open in red_output_append_out_tar");
        return -1;
    }
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
    if(fd == -1){
        perror("open in red_output_trunc_out_tar");
        return -1;
    }
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
    if(fd == -1){
        perror("open in red_err_append_out_tar");
        return -1;
    }
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
    if(fd == -1){
        perror("open in red_err_trunc_out_tar");
        return -1;
    }
    int ret_dup2 = dup2(fd, STDERR_FILENO);
    if(ret_dup2 == -1){
        close(fd);
        perror("dup2 in red_err_trunc_out_tar");
        return -1;
    }
    return fd;
}

int red_input_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int red_output_append_in_tar(const char *path_tar, const char *path_in_tar, char *buf_cmd, int size_buf_cmd){
    int pos[2];
    int fd_tar = open(path_tar, O_RDWR);

    if(fd_tar == -1){
        perror("open in red_output_append_in_tar");
        return -1;
    }

    int ret = pos_file_in_tar(fd_tar, path_in_tar, pos);
    if(ret == -1){
        return -1;
    }

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(size_tar == -1){
        perror("lseek in red_input_in_tar");
        return -1;
    }

    int ret_lseek = lseek(fd_tar, pos[1], SEEK_SET); // end of file (path_in_tar file)
    if(ret_lseek == -1){
        perror("lseek in red_input_in_tar");
        return -1;
    }

    char *buf = malloc(sizeof(char) * (size_tar - ret_lseek));
    memset(buf, '\0', sizeof(char) * (size_tar - ret_lseek));
    int rd = read(fd_tar, buf, sizeof(char) * (size_tar - ret) * BLOCK_SIZE);
    if(rd == -1){
        perror("read in red_output_append_in_tar");
        return -1;
    }

    ret_lseek = lseek(fd_tar, pos[0], SEEK_SET); // beginning of file (path_in_tar file)
    if(ret_lseek == -1){
        perror("lseek in red_output_append_in_tar");
        return -1;
    }

    int wr = write(fd_tar, buf_cmd, sizeof(char) * size_buf_cmd);
    if(wr == -1){
        perror("write in red_output_append_in_tar");
        return -1;
    }

    wr = write(fd_tar, buf, sizeof(char) * (size_tar - ret_lseek) * BLOCK_SIZE);
    if(wr == -1){
        perror("write in red_output_append_in_tar");
        return -1;
    }

    // changing file header
    int size_file = str_length(buf_cmd);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(ret_lseek == -1){
        perror("lseek in red_output_append_in_tar");
        return -1;
    }
    struct posix_header header;
    rd = read(fd_tar, &header, BLOCK_SIZE);
    if(rd == -1){
        perror("read in red_output_append_in_tar");
        return -1;
    }
    //sprintf(header.size, "%011o", size_file); //Line MacOS
    sprintf(header.size, "%011o", size_file);
    set_checksum(&header);
    ret_lseek = lseek(fd_tar, pos[0] - BLOCK_SIZE, SEEK_SET);
    if(ret_lseek == -1){
        perror("lseek in red_output_append_in_tar");
        return -1;
    }
    wr = write(fd_tar, &header, BLOCK_SIZE);
    if(wr == -1){
        perror("write in red_output_append_in_tar");
        return -1;
    }

    return 0;
}

int red_output_trunc_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int red_err_append_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int red_err_trunc_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}