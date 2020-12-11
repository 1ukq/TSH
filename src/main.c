#include "copy.h"
#include "shell.h"
#include "list_file.h"
#include "types/posix_header.h"
#include "redirection.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){

    //shell();

    // exemple exécution redirection
    /*
    int fildes[2];
    pipe(fildes);
    int f = fork();
    if(f == 0){
        close(fildes[0]);
        red_err_to_pipe(fildes[1]);
        execlp("cat", "cat", "a", NULL);
    }
    if(f > 0){
        close(fildes[1]);
        char *ret = malloc(BLOCK_SIZE);
        int n = buffarize_output(fildes[0], ret);
        red_err_trunc_in_tar("test.tar", "c", ret, n);
        close(fildes[0]);
        //printf("%s\n", ret);
    }
    */

    // exemple redirection input avec tar
    /*
    int fildes[2];
    pipe(fildes);
    input_to_pipe(fildes[1], "test.tar", "a");
    red_input_in_tar(fildes[0]);
    close(fildes[1]);
    close(fildes[0]);
    execlp("cat", "cat", NULL);
    */

    return 0;

}