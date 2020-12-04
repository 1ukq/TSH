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

void copy_from_tar_test(const char *path_tar, const char *path_file_source_in_tar, const char *path_file_source, const char *path_file_dest, int file_source_size){

    int fd_dest = open(path_file_dest, O_RDWR|O_CREAT|O_TRUNC, 0000700);

    //char *str = "targets/";
    //char root[strlen(str) + strlen(path_file_source) + 1];
    //strcpy(root, str);

    int ret_copy_from_tar = copy_from_tar(path_tar, path_file_source_in_tar, fd_dest);
    if(ret_copy_from_tar == -1){
        //perror("copy_from_tar");
        //return;
    }

    char *buf_source = malloc(file_source_size);
    char *buf_dest = malloc(file_source_size);
    //On concatène "targets" et path_file_source parce path_file_source correspond au chemin absolu de file_source à l'intérieur d'un tarball
    //strcat(root, path_file_source);
    //printf("%s\n", root);
    int fd_source = open(path_file_source, O_RDONLY);
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

    printf("+++++test for copy_file_from_tar passed+++++++\n");

    close(fd_dest);
    close(fd_source);
    free(buf_dest);
    free(buf_source);
}

int main(void){

    system("bash script_rm.sh");
    system("bash script.sh");

    copy_from_tar_test("targets/test.tar", "targets/bar", "targets/bar", "targets/dest/bar", 2105);
    copy_from_tar_test("targets/test.tar", "targets/test_dir/foo", "targets/test_dir/foo", "targets/dest/foo", 19); 
    copy_from_tar_test("targets/test.tar", "targets/test_dir/helloworld", "targets/test_dir/helloworld", "targets/dest/helloworld", 13);

    system("bash script_rm.sh");

   return 0;
}
