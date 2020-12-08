#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils/utils_red.h"

int red_input_out_tar(const char *file);

int red_output_append_out_tar(const char *file);

int red_output_trunc_out_tar(const char *file);

int red_err_append_out_tar(const char *file);

int red_err_trunc_out_tar(const char *file);

int red_input_in_tar(const char *path_tar, const char *path_in_tar);

int red_output_append_in_tar(const char *path_tar, const char *path_in_tar);

int red_output_trunc_in_tar(const char *path_tar, const char *path_in_tar);

int red_err_append_in_tar(const char *path_tar, const char *path_in_tar);

int red_err_trunc_in_tar(const char *path_tar, const char *path_in_tar);