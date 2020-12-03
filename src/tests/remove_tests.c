#include "munit/munit.h"
#include "../remove.h"

void remove_file_from_tar_test(const char *path_tar, const char *path_file_source, int size_tar, char *expected_content){

    int ret = remove_file_from_tar(path_tar, path_file_source);
    int fd_tar = open(path_tar, O_RDONLY);
    munit_assert_int(ret, ==, 0);
    char *content = malloc(sizeof(char) * size_tar);
    read(fd_tar, content, size_tar);
    if(content[6673] == '\n') printf("0\n");
    munit_assert_memory_equal(size_tar, expected_content, content);
    printf("++++test for remove_file_from_tar passed++++++\n");

}

int main(void){

    system("bash script_rm.sh");
    system("bash script.sh");

    int size_tar = 3584;
    char *expected_content = malloc(sizeof(char) * size_tar);
    int fd_expected_tar = open("targets/expected_remove_file_from_tar_test.tar", O_RDONLY);
    read(fd_expected_tar, expected_content, 10240);
    remove_file_from_tar_test("targets/test.tar", "targets/bar", size_tar, expected_content);
    return 0;

    system("bash script_rm.sh");

}