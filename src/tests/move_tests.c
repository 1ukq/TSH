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

void find_last_block_test(int fd_tar, struct stat *restrict buf_stat, int expected_ret){

    int ret = find_last_block(fd_tar, buf_stat);
    munit_assert_int(ret, ==, expected_ret);
    printf("+++++test for find_last_block function passed++++\n");

}

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
    munit_assert_memory_equal(size, buf, expected_content);
    munit_assert_int(size, ==, strlen(expected_content));
    printf("++test for insert_file_in_tar function passed++\n");

}

void suppress_file_test(int fd_tar, int pos_from, int pos_to, int size_tar, char *expected_content){

    int ret = suppress_file(fd_tar, pos_from, pos_to, size_tar);
    munit_assert_int(ret, ==, 0);
    char *content = malloc(sizeof(char) * size_tar);
    lseek(fd_tar, 0, SEEK_SET); //Don't know why without this line, test does not pass on alpine docker
    read(fd_tar, content, size_tar);
    munit_assert_memory_equal(size_tar, expected_content, content);
    printf("+++++++++test for suppress_file_passed+++++++++\n");

}

int main(int argc, char *argv[]){

    system("bash script_rm.sh");
    system("bash script.sh");

    int expected_ret = 11;
    int fd_tar_f = open("targets/test.tar", O_RDONLY);
    struct stat stat_tar;
    stat("targets/test.tar", &stat_tar);
    find_last_block_test(fd_tar_f, &stat_tar, expected_ret);

    system("bash script_rm.sh");
    system("bash script.sh");

    char *expected_buf = malloc(sizeof(char) * 2105);
    int fd = open("targets/bar", O_RDONLY);
    read(fd, expected_buf, 2105);
    buffarize_test("targets/bar", expected_buf, 2105);
    free(expected_buf);
    close(fd);

    system("bash script_rm.sh");
    system("bash script.sh");

    char *expected_content_insert = "Hello world !";
    char *expected_name = "dir/helloworld";
    insert_file_in_tar_test("targets/test.tar", "targets/test_dir/helloworld", "dir/", expected_name, expected_content_insert);

    system("bash script_rm.sh");
    system("bash script.sh");

    int fd_tar = open("targets/test.tar", O_RDWR);
    int fd_sup_tar = open("targets/expected_suppress_test.tar", O_RDONLY);
    int pos_from = 0;
    int pos_to = 6 * BLOCK_SIZE;
    int size_tar = 10240;
    char *expected_content_sup = malloc(sizeof(char) * size_tar);
    read(fd_sup_tar, expected_content_sup, sizeof(char) * size_tar);
    suppress_file_test(fd_tar, pos_from, pos_to, size_tar, expected_content_sup);
    free(expected_content_sup);
    close(fd_tar);
    close(fd_sup_tar);

    system("bash script_rm.sh");

    return 0;

}
