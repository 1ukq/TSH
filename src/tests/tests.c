#include "munit/munit.h"
#include "../copy.h"
#include "../list_file.h"
#include "../mkdir.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

void copy_test(const char *path_tar, const char *path_file_source, const char *path_file_dest, int file_source_size){

    int fd_dest = open(path_file_dest, O_RDWR|O_CREAT|O_TRUNC, 0000700);

    char *str = "targets/";
    char root[strlen(str) + strlen(path_file_source) + 1];
    strcpy(root, str);

    int ret_copy = copy(path_tar, path_file_source, fd_dest);
    if(ret_copy == -1){
        perror("copy");
        return;
    }

    char *buf_source = malloc(file_source_size);
    char *buf_dest = malloc(file_source_size);
    //On concatène "targets" et path_file_source parce path_file_source correspond au chemin absolu de file_source à l'intérieur d'un tarball
    strcat(root, path_file_source);
    int fd_source = open(root, O_RDONLY);
    fd_dest = open(path_file_dest, O_RDONLY);

    int size_source = read(fd_source, buf_source, file_source_size);
    if(size_source == -1){
        perror("read 1");
        return;
    }

    int size_dest = read(fd_dest, buf_dest, file_source_size);
    if(size_dest == -1){
        perror("read 2");
        return;
    }

    munit_assert_int(file_source_size, ==, ret_copy);
    munit_assert_memory_equal(file_source_size, buf_source, buf_dest);

    close(fd_dest);
    close(fd_source);
    free(buf_dest);
    free(buf_source);
}

void cat_test(const char *path_tar, const char *path_file_source){

    copy(path_tar, path_file_source, STDOUT_FILENO);
    printf("\n");

}

void ls_test(const char *path_tar, const char *path_file_source, int expected_nb_files){

    int ret = ls(STDOUT_FILENO, ' ',path_tar, path_file_source);
    munit_assert_int(expected_nb_files, ==, ret);
}

int main(void){

    copy_test("targets/test.tar", "bar", "targets/dest", 2105);
    copy_test("targets/test.tar", "test_dir/foo", "targets/dest", 19); 
    copy_test("targets/test.tar", "test_dir/helloworld", "targets/dest", 13);
    
    cat_test("targets/test.tar", "bar");
    cat_test("targets/test.tar", "test_dir/helloworld");
    cat_test("targets/test.tar", "test_dir/foo");

    ls_test("targets/test.tar", "test_dir/", 2);
    ls_test("targets/test.tar", "", 2);

   return 0;
}
