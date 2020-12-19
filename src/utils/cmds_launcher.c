#include "cmds_launcher.h"

int cmds_launcher(char ***cmds, int red_type, char *file){

    int ret_pipe = 0;
    int ret_dup2 = 0;

    int it = 0;
    int old_fildes[2];
    int new_fildes[2];
    int red_fildes[2];

    while(cmds[it] != NULL){
        if(cmds[it + 1] != NULL){
            ret_pipe = pipe(new_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
        }
        if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
            ret_pipe = pipe(red_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
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
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
                close(red_fildes[0]);
                ret_dup2 = dup2(red_fildes[1], STDOUT_FILENO);
                if(check_sys_call(ret_dup2, "dup2 in cmds_launcher") == -1) return -1;
                close(red_fildes[1]);
            }
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_OUT_TAR || red_type == RED_OUT_TRUNC_OUT_TAR)){
                int fd;
                if(red_type == RED_OUT_APPEND_OUT_TAR) fd = red_output_append_out_tar(file);
                else if(red_type == RED_OUT_TRUNC_OUT_TAR) fd = red_output_trunc_out_tar(file);
                close(fd);
            }
            execvp(cmds[it][0], cmds[it]);
        }
        else{
            wait(NULL);
            if(it > 0){
                close(old_fildes[0]);
                close(old_fildes[1]);
            }
            if(cmds[it + 1] != NULL){
                old_fildes[0] = new_fildes[0];
                old_fildes[1] = new_fildes[1];
            }
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
                close(red_fildes[1]);
                char *buf = malloc(BLOCK_SIZE);
                int r = buffarize_output(red_fildes[0], buf);
                close(red_fildes[0]);
                printf("%d\n", r);
                if(red_type == RED_OUT_APPEND_IN_TAR) red_append_in_tar("test.tar", "b", buf, r);
                else if(red_type == RED_OUT_TRUNC_IN_TAR) red_trunc_in_tar("test.tar", "b", buf, r);
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

int main(){

    char *s1 = "ls";
    char *op = "-l";
    //char *f = "direeee";

    char *s2 = "head";

    char *file = "a";

    char *ss1[3];
    char *ss2[2];
    char *ss3[2];

    ss1[0] = s1;
    ss1[1] = op;
    ss1[2] = NULL;
    //ss1[3] = NULL;

    ss2[0] = s2;
    ss2[1] = NULL;

    ss3[0] = file;
    ss3[1] = NULL;

    char ***cmds = malloc(sizeof(char**) * 3);
    cmds[0] = ss1;
    cmds[1] = ss2;
    cmds[2] = NULL;
    //cmds[3] = NULL;
    cmds_launcher(cmds, RED_OUT_TRUNC_OUT_TAR, file);
    execlp("ls", "ls", NULL);

}
