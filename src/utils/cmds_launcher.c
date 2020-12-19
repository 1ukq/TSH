#include "cmds_launcher.h"

int cmds_launcher(char ***cmds, int red_type, char *file){

    int ret_pipe = 0;
    int ret_dup2 = 0;

    int it = 0;
    int old_fildes[2];
    int new_fildes[2];
    int red_fildes[2];
    int err_fildes[2];

    while(cmds[it] != NULL){

        if(cmds[it + 1] != NULL){
            ret_pipe = pipe(new_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
        }

        if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
            ret_pipe = pipe(red_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
        }

        if(cmds[it + 1] == NULL && (red_type == RED_ERR_APPEND_IN_TAR || red_type == RED_ERR_TRUNC_IN_TAR)){
            ret_pipe = pipe(err_fildes);
            if(check_sys_call(ret_pipe, "pipe in cmds_launcher") == -1) return -1;
        }

        int n = fork();
        if(check_sys_call(n, "fork in cmds_launcher") == -1) return -1;
        if(n == 0){ // Fils

            // Gestion de la série de pipes
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

            //Redirection > et >> en dehors ou dans tar : écriture dans un nouveau pipe de l'output si redirection dans tar
            // sinon redirection de l'output dans file
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
                close(red_fildes[0]);
                ret_dup2 = dup2(red_fildes[1], STDOUT_FILENO);
                if(check_sys_call(ret_dup2, "dup2 in cmds_launcher") == -1) return -1;
                close(red_fildes[1]);
            }
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_OUT_TAR || red_type == RED_OUT_TRUNC_OUT_TAR)){
                int fd = 0;
                if(red_type == RED_OUT_APPEND_OUT_TAR) fd = red_output_append_out_tar(file);
                else if(red_type == RED_OUT_TRUNC_OUT_TAR) fd = red_output_trunc_out_tar(file);
                close(fd);
            }

            //Redirection 2> et 2>> en dehors ou dans tar : écriture dans un nouveau pipe de l'output si redirection dans tar
            // sinon redirection de l'output dans file
            if(red_type == RED_ERR_APPEND_OUT_TAR && cmds[it + 1] == NULL){
                int fd = red_err_append_out_tar(file);
                close(fd);
            }
            if(red_type == RED_ERR_TRUNC_OUT_TAR && cmds[it + 1] == NULL){
                int fd = red_err_trunc_out_tar(file);
                close(fd);
            }
            if(cmds[it + 1] == NULL && (red_type == RED_ERR_APPEND_IN_TAR || red_type == RED_ERR_TRUNC_IN_TAR)){
                close(err_fildes[0]);
                ret_dup2 = dup2(err_fildes[1], STDERR_FILENO);
                close(err_fildes[1]);
            }

            execvp(cmds[it][0], cmds[it]);
        }
        else{ // Père

            //Gestion de la série de pipes
            if(it > 0){
                close(old_fildes[0]);
                close(old_fildes[1]);
            }
            if(cmds[it + 1] != NULL){
                old_fildes[0] = new_fildes[0];
                old_fildes[1] = new_fildes[1];
            }

            //Redirection > et >> dans tar : création d'un buffer pour accueillir le contenu du pipe de redirection
            // puis écriture dans le tar (au bon endroit)
            if(cmds[it + 1] == NULL && (red_type == RED_OUT_APPEND_IN_TAR || red_type == RED_OUT_TRUNC_IN_TAR)){
                close(red_fildes[1]);
                char *buf = malloc(BLOCK_SIZE);
                int r = buffarize_output(red_fildes[0], buf);
                close(red_fildes[0]);
                if(red_type == RED_OUT_APPEND_IN_TAR) red_append_in_tar("test.tar", "b", buf, r);
                else if(red_type == RED_OUT_TRUNC_IN_TAR) red_trunc_in_tar("test.tar", "b", buf, r);
            }

            //Redirection 2> et 2>> dans tar : création d'un buffer pour accueillir le contenu du pipe de redirection
            // puis écriture dans le tar (au bon endroit)
            if(cmds[it + 1] == NULL && (red_type == RED_ERR_APPEND_IN_TAR || red_type == RED_ERR_TRUNC_IN_TAR)){
                close(err_fildes[1]);
                char *buf = malloc(BLOCK_SIZE);
                int r = buffarize_output(err_fildes[0], buf);
                printf("%d\n", r);
                close(err_fildes[0]);
                if(red_type == RED_ERR_APPEND_IN_TAR) red_append_in_tar("test.tar", "b", buf, r);
                else if(red_type == RED_ERR_TRUNC_IN_TAR) red_trunc_in_tar("test.tar", "b", buf, r);
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
