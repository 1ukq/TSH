#include "../copy.h"

int main(){

    copy_from_tar_to_tar_r("test1.tar", "test2.tar", "dir", "c");
    return 0;

}