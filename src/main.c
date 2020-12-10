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
    int fildes[2];
    pipe(fildes);
    int f = fork();
    if(f == 0){
        close(fildes[0]);
        red_output_to_pipe(fildes[1]);
        execlp("cat", "cat", "a", NULL);
    }
    if(f > 0){
        close(fildes[1]);
        char *ret = malloc(BLOCK_SIZE);
        int n = buffarize_output(fildes[0], ret);
        red_output_trunc_in_tar("test.tar", "b", ret, n);
        //printf("%s\n", ret);
    }
    return 0;

}