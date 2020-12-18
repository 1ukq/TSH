#include "cmds_launcher.h"

int cmds_launcher(char ***cmds){

    int ret_pipe = 0;
    int ret_dup2 = 0;

    int it = 0;
    int old_fildes[2];
    int new_fildes[2];
    while(cmds[it] != NULL){
        if(cmds[it + 1] != NULL){
            ret_pipe = pipe(new_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
        }
        if(it == 0){
            old_fildes[0] = new_fildes[0];
            old_fildes[1] = new_fildes[1];
        }
        int n = fork();
        if(check_sys_call(n, "fork in cmds_launcher") == -1) return -1;
        if(n == 0){
            if(it > 0){
                ret_dup2 = dup2(old_fildes[0], STDIN_FILENO);
                if(check_sys_call(ret_dup2, "dup2 in cmds_launcher") == -1) return -1;
                close(old_fildes[0]);
                close(old_fildes[1]);
            }
            if(cmds[it + 1] != NULL){
                close(new_fildes[0]);
                ret_dup2 = dup2(new_fildes[1], STDOUT_FILENO);
                if(check_sys_call(ret_dup2, "dup2 in cmds_launcher") == -1) return -1;
                close(new_fildes[1]);
            }
            execvp(cmds[it][0], cmds[it]);
        }
        else{
            if(it > 0){
                close(old_fildes[0]);
                close(old_fildes[1]);
            }
            if(cmds[it + 1] != NULL){
                old_fildes[0] = new_fildes[0];
                old_fildes[1] = new_fildes[1];
            }
        }

        it++;


    }

    if(cmds[1] != NULL){
        close(old_fildes[0]);
        close(old_fildes[1]);
    }

    return 0;

}