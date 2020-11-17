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

#define BLOCK_SIZE 512

void buffarize_test(const char *path_file_source, char *expected_buf, off_t nb_bytes){

    struct stat buf;
    stat(path_file_source, &buf);
    char *file_source_buf = buffarize(path_file_source, &buf);
    munit_assert_memory_equal(nb_bytes, expected_buf, file_source_buf);
    printf("++++++test for buffarize function passed+++++++\n");

}

void insert_file_in_tar_test(const char *path_tar, const char *path_file_source, char *path_in_tar, const char *expected_name, const char *expected_content){

    int ret = insert_file_in_tar(path_tar, path_file_source, path_in_tar);
    munit_assert_int(ret, ==, 0);
    int fd_tar = open(path_tar, O_RDONLY);
    struct posix_header header;
    read(fd_tar, &header, BLOCK_SIZE);
    int size = 0;
    int shift = 0;
    while(strcmp(header.name, expected_name)){
        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
        read(fd_tar, &header, BLOCK_SIZE);
    }
    sscanf(header.size, "%o", &size);
    char *buf = malloc(sizeof(char) * size);
    read(fd_tar, buf, size);
    if(buf[size - 1] == '\n') printf("0\n");
    munit_assert_memory_equal(size - 1, buf, expected_content);
    munit_assert_int(size, ==, strlen(expected_content));
    printf("++test for insert_file_in_tar function passed++\n");

}

void suppress_file_test(int fd_tar, int pos_from, int pos_to, int size_tar){

    int ret = suppress_file(fd_tar, pos_from, pos_to, size_tar);
    munit_assert_int(ret, ==, 0);
    printf("+++++++++test for suppress_file_passed+++++++++\n");

}

void mv_from_tar_to_tar_test(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, char *path_in_tar){

    int ret = mv_from_tar_to_tar(path_tar_source, path_tar_target, path_file_source, path_in_tar);
    munit_assert_int(ret, ==, 0);
    printf("++++++test for mv_from_tar_to_tar passed+++++++\n");

}

void mv_from_dir_to_tar_test(const char *path_tar, const char *path_file_source, char *path_in_tar){

    int ret = mv_from_dir_to_tar(path_tar, path_file_source, path_in_tar);
    munit_assert_int(ret, ==, 0);
    printf("+++++++test for mv_from_dir_to_tar passed++++++\n");

}

void mv_from_tar_to_dir_test(const char *path_tar, const char *path_file_source, char *path_dest){

    int ret = mv_from_tar_to_dir(path_tar, path_file_source, path_dest);
    munit_assert_int(ret, ==, 0);
    printf("+++++++test for mv_from_tar_to_dir passed++++++\n");

}

int main(int argc, char *argv[]){

    system("./script_rm.sh");
    system("./script.sh");

    char *expected_buf = malloc(sizeof(char) * 2105);
    int fd = open("targets/bar", O_RDONLY);
    read(fd, expected_buf, 2105);
    buffarize_test("targets/bar", expected_buf, 2105);
    free(expected_buf);
    close(fd);

    system("./script_rm.sh");
    system("./script.sh");

    char *expected_content = "Hello world !";
    char *expected_name = "dir/helloworld";
    insert_file_in_tar_test("targets/test.tar", "targets/test_dir/helloworld", "dir/", expected_name, expected_content);

    system("./script_rm.sh");
    system("./script.sh");

    int fd_tar = open("targets/test.tar", O_RDONLY);
    int pos_from = 0;
    int pos_to = 6 * BLOCK_SIZE;
    int size_tar = 10240;
    suppress_file_test(fd_tar, pos_from, pos_to, size_tar);

    system("./script_rm.sh");
    system("./script.sh");

    //mv_from_tar_to_tar_test();

    system("./script_rm.sh");
    system("./script.sh");

    //mv_from_dir_to_tar_test();

    system("./script_rm.sh");
    system("./script.sh");

    //mv_from_tar_to_dir_test();

    return 0;

}