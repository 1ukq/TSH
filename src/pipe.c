#include "pipe.h"

//If the commands does note have any option you can set opt1 and opt2 to NULL
int pipe_tsh(const char *cmd1, const char *opt1, const char *cmd2, const char *opt2){
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
            execlp(cmd1, cmd1, opt1, NULL);
            break;
    }
    return 0;
}