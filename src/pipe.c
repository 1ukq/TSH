#include "pipe.h"


int pipe_tsh(const char *cmd1, const char *cmd2){

    int fildes[2];
    int ret_pipe = pipe(fildes);
    if(ret_pipe == -1){
        perror("pipe in pipe_tsh");
        return -1;
    }
    switch(fork()){
        case -1 : 
            perror("fork"); 
            return -1;
            break;
        case 0 :
            close(fildes[1]);
            break;
        default :
            close(fildes[0]);
            break;
    }
    return 0;

}

int main(){
    return 0;
}