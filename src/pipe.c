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
        case 0 : //fils, read
            close(fildes[1]);
            dup2(fildes[0], STDIN_FILENO);
            execlp(cmd2, cmd2, NULL);
            break;
        default : //père, write
            close(fildes[0]);
            dup2(fildes[1], STDOUT_FILENO);
            execlp(cmd1, cmd1, NULL);
            break;
    }
    return 0;

}

int main(){
    pipe_tsh("yes", "head");
    return 0;
}