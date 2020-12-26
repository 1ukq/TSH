#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../utils/find_file.h"
#include "../utils/utils_string.h"

//redirection with tar
int buffarize_output(int fd_read_end, char *buffer);

int input_to_pipe(int fd_write_end, char *path_tar, const char *path_in_tar);

int red_input_in_tar(int fd_read_end);

int red_trunc_in_tar(const char *path_tar, const char *path_in_tar, char *buf_cmd, int size_buf_cmd);

int red_append_in_tar(const char *path_tar, const char *path_in_tar, char *buf_cmd, int size_buf_cmd);


//redirection without tar
int red_input_out_tar(const char *file);

int red_output_append_out_tar(const char *file);

int red_output_trunc_out_tar(const char *file);

int red_err_append_out_tar(const char *file);

int red_err_trunc_out_tar(const char *file);