#include "munit/munit.h"
#include "../copy.h"
#include "../list_file.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 4096

void copy_test(const char *path_tar, const char *path_file_source, const char *path_file_dest, int file_source_size){

    char *str = "targets/";
    char root[strlen(str) + strlen(path_file_source) + 1];
    strcpy(root, str);

    int ret_copy = copy(path_tar, path_file_source, path_file_dest);
    if(ret_copy == -1){
        perror("copy");
        return;
    }

    char *buf_source = malloc(BUF_SIZE);
    char *buf_dest = malloc(BUF_SIZE);
    //On concatène "targets" et path_file_source parce path_file_source correspond au chemin absolu de file_source à l'intérieur d'un tarball
    strcat(root, path_file_source);
    int fd_source = open(root, O_RDONLY);
    int fd_dest = open(path_file_dest, O_RDONLY);

    int size_source = read(fd_source, buf_source, BUF_SIZE);
    if(size_source == -1){
        perror("read 1");
        return;
    }

    int size_dest = read(fd_dest, buf_dest, BUF_SIZE);
    if(size_dest == -1){
        perror("read 2");
        return;
    }

    munit_assert_int(file_source_size, ==, ret_copy);
    munit_assert_memory_equal(BUF_SIZE, buf_source, buf_dest);

    close(fd_dest);
    close(fd_source);
    free(buf_dest);
    free(buf_source);
}

void ls_test(const char *path_tar, const char *path_file_source, const char *path_file_dest, char *expected_list){

    int fd_dest = open(path_file_dest, O_RDWR|O_TRUNC);
    dup2(fd_dest, STDOUT_FILENO);
    int ret = ls(path_tar, path_file_source);

    char *buf = malloc(ret + 1);
    int n = write(fd_dest, buf, ret);
    buf[ret] = '\0';
    printf("%s\n", buf);

    munit_assert_int(5, ==, ret);
    //munit_assert_memory_equal(ret + 1, buf, expected_list);
}

int main(void){

   copy_test("targets/test.tar", "bar", "targets/dest", 2105);
   copy_test("targets/test.tar", "test_dir/foo", "targets/dest", 19); 
   copy_test("targets/test.tar", "test_dir/helloworld", "targets/dest", 13);

   ls_test("targets/test.tar", "", "targets/dest", "bar	");

   return 0;
}