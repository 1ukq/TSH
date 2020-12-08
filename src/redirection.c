#include "redirection.h"

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

int red_output_append_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int red_output_trunc_in_tar(const char *path_tar, const char *path_in_tar){

    //init
    int fd_tar = open(path_tar, O_RDWR);
    if(fd_tar == -1){
        perror("open in red_output_trunc_in_tar");
        return -1;
    }
    int *pos = malloc(2 * sizeof(int));
    pos_file_in_tar(fd_tar, path_in_tar, pos);
    int size_file = pos[1] - pos[0];

    //trunc file
    char *sup_buf = malloc(sizeof(char) * size_file);
    memset(sup_buf, '\0', size_file);
    int l = lseek(fd_tar, pos[0], SEEK_SET);
    if(l == -1){
        perror("lseek in red_output_trunc_in_tar");
        return -1;
    }
    int wr = write(fd_tar, sup_buf, size_file);
    if(wr == -1){
        perror("write in red_output_trunc_in_tar");
        return -1;
    }

    //reset pos
    l = lseek(fd_tar, pos[0], SEEK_SET);
    if(l == -1){
        perror("lseek in red_output_trunc_in_tar");
        return -1;
    }
    dup2(fd_tar, STDOUT_FILENO);
    free(pos);
    return 0;
}

int red_err_append_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int red_err_trunc_in_tar(const char *path_tar, const char *path_in_tar){
    return 0;
}

int main(){

    red_output_trunc_in_tar("test.tar", "c");
    execlp("cat", "cat", "a", NULL);

}