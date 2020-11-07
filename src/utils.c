#include "utils.h"

int name_length(const char* path_file_source){

    int name_length = 0;
    for(int i = strlen(path_file_source) - 1; i > -1; i--){
        if(path_file_source[i] == '\0') continue;
        if(path_file_source[i] != '/') name_length++;
        else break;
    }
    return name_length;

}

char *name(const char *path){

    int length = name_length(path);
    int len_path = strlen(path);
    char *ret = malloc(sizeof(char) * length);
    if(ret == NULL) return NULL;
    for(int i = 0; i < length; i++) ret[i] = path[len_path - length + i];
    return ret;

}

char *concatenate(char *str1, char *str2){

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char *ret = malloc(sizeof(char) * (len1 + len2 + 1));
    if(ret == NULL) return NULL;
    for(int i = 0; i < len1; i++) ret[i] = str1[i];
    for(int i = 0; i < len2; i++) ret[i + len1] = str2[i];
    ret[len1 + len2] = '\0';
    return ret;

}

