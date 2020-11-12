#include "munit/munit.h"
#include "../copy.h"
#include "../list_file.h"
#include "../makedir.h"
#include "../types/posix_header.h"
#include "../remove.h"
#include "../utils/utils_string.h"
#include "../move.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/utsname.h>

void buffarize_test(const char *path_file_source, char *expected_buf, off_t nb_bytes){

    struct stat buf;
    stat(path_file_source, &buf);
    char *file_source_buf = buffarize(path_file_source, &buf);
    munit_assert_memory_equal(nb_bytes, expected_buf, file_source_buf);

}

int main(int argc, char *argv[]){

    char *expected_buf = malloc(sizeof(char) * 2105);
    int fd = open("targets/bar", O_RDONLY);
    read(fd, expected_buf, 2105);
    buffarize_test("targets/bar", expected_buf, 2105);
    free(expected_buf);
    close(fd);
    
}