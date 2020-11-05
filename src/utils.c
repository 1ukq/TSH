#include "utils.h"

int name_length(const char* path_file_source){

    int name_length = 0;
    for(int i = strlen(path_file_source) - 1; i > -1; i--){
        if(path_file_source[i] != '/') name_length++;
        else break;
    }
    return name_length;

}

void get_name_from_path(const char *path_file_source, char *file_name){

    int length = name_length(path_file_source);
    strncpy(file_name, path_file_source + (strlen(path_file_source) - length), length);

}

void concatenate_name_path(const char* path_file_source, const char *path_in_tar, char *file_name, char *buf){

    strncpy(buf, path_in_tar, strlen(path_in_tar));
    strcat(buf, file_name);

}