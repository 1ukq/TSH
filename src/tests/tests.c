#include "munit/munit.h"
#include "../copy.h"
#include "../list_file.h"
#include "../types/posix_header.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

void copy_from_tar_test(const char *path_tar, const char *path_file_source, const char *path_file_dest, int file_source_size){

    int fd_dest = open(path_file_dest, O_RDWR|O_CREAT|O_TRUNC, 0000700);

    char *str = "targets/";
    char root[strlen(str) + strlen(path_file_source) + 1];
    strcpy(root, str);

    int ret_copy_from_tar = copy_from_tar(path_tar, path_file_source, fd_dest);
    if(ret_copy_from_tar == -1){
        perror("copy_from_tar");
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

    munit_assert_int(file_source_size, ==, ret_copy_from_tar);
    munit_assert_memory_equal(file_source_size, buf_source, buf_dest);

    close(fd_dest);
    close(fd_source);
    free(buf_dest);
    free(buf_source);
}

void cat_test(const char *path_tar, const char *path_file_source){

    copy_from_tar(path_tar, path_file_source, STDOUT_FILENO);
    printf("\n");

}

void ls_test(const char *path_tar, const char *path_file_source, int expected_nb_files){

    int ret = ls(path_tar, path_file_source);
    munit_assert_int(expected_nb_files, ==, ret);
}

void buffarize_test(const char *path_file_source, char *expected_buf, off_t nb_bytes){

    struct stat buf;
    stat(path_file_source, &buf);
    char *file_source_buf = buffarize(path_file_source, &buf);
    munit_assert_memory_equal(nb_bytes, expected_buf, file_source_buf);

}

int main(void){

    copy_from_tar_test("targets/test.tar", "bar", "targets/dest", 2105);
    copy_from_tar_test("targets/test.tar", "test_dir/foo", "targets/dest", 19); 
    copy_from_tar_test("targets/test.tar", "test_dir/helloworld", "targets/dest", 13);



    
    cat_test("targets/test.tar", "bar");
    printf("\n");
    cat_test("targets/test.tar", "test_dir/helloworld");
    printf("\n");
    cat_test("targets/test.tar", "test_dir/foo");
    printf("\n");




    ls_test("targets/test.tar", "test_dir/", 2);
    printf("\n");
    ls_test("targets/test.tar", "", 2);
    printf("\n");



    char *expected_buf = malloc(sizeof(char) * 2105);
    int fd = open("targets/bar", O_RDONLY);
    read(fd, expected_buf, 2105);
    buffarize_test("targets/bar", expected_buf, 2105);
    free(expected_buf);
    close(fd);





    struct stat buf;
    //stat("Makefile", &buf);
    struct posix_header header;
    //memset(&header, '\0', sizeof(char) * BLOCK_SIZE);


    /*
    int ret = create_file_header("bar", &buf, &header);
    printf("ret : %d\n", ret);
    printf("name : %s\n", header.name);
    printf("size : %s\n", header.size);
    printf("type : %c\n", header.typeflag);
    printf("version : %s\n", header.version);
    printf("magic : %s\n", header.magic);
    printf("mode : %s\n", header.mode);
    printf("chcksum : %s\n", header.chksum);
    */
    //printf("uname : %s\n", header.uname);

    //char *p = (char *)&header;
    //for(int i = 0; i < BLOCK_SIZE; i++) printf("%c", p[i]);


    //int f = open("test", O_WRONLY);
    //write(f, &header, 512);

    insert_file_in_tar("a.tar", "targets/test_dir/foo", "foo");

   return 0;
}