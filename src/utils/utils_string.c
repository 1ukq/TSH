#include "utils_string.h"

int compare_buffers_of_same_size(char *buf1, char*buf2, int nbytes){

    for(int i = 0; i < nbytes; i++) if(buf1[i] != buf2[i]) return 0;
    return 1;

}

void copy_string(char *str1, char *str2, int length){

    for(int i = 0; i < length; i++){
        str1[i] = str2[i];
    }

}

int str_length(char *str){
    int i = str[0];
    int ret = 0;
    while(i != '\0'){
        ret++;
        i = str[ret];
    }
    return ret;
}

int name_length(const char* path_file_source){

    int name_length = 0;
    for(int i = strlen(path_file_source) - 1; i > -1; i--){
        if(path_file_source[i] == '\0') continue;
        if(path_file_source[i] != '/') name_length++;
        else break;
    }
    return name_length;

}

int name_dir_length(const char *path_dir_src){

    int name_length = 0;
    for(int i = strlen(path_dir_src) - 1; i > -1; i--){
        if(path_dir_src[i] == '\0') continue;
        else if(path_dir_src[i] != '/') name_length++;
        else if(path_dir_src[i] == '/' && name_length == 0) name_length++;
        else if(path_dir_src[i] == '/') break;
    }
    return name_length;

}

char *name(const char *path){

    int length = name_length(path);
    int len_path = strlen(path);
    char *ret = malloc(sizeof(char) * (length + 1));
    if(ret == NULL) return NULL;
    for(int i = 0; i < length; i++) ret[i] = path[len_path - length + i];
    ret[length] = '\0';
    return ret;

}

char *name_dir(const char *path){

    int length = name_dir_length(path);
    int len_path = strlen(path);
    char *ret = malloc(sizeof(char) * (length + 1));
    if(ret == NULL) return NULL;
    for(int i = 0; i < length; i++) ret[i] = path[len_path - length + i];
    ret[length] = '\0';
    return ret;

}

char *get_path_dir(const char *path){

    int name_length = name_dir_length(path);
    char *ret = malloc(strlen(path) - name_length + 1);
    memcpy(ret, path, strlen(path) - name_length);
    ret[strlen(path) - name_length + 1] = '\0';
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

int depth_dir(const char *path){
    int it = 0;
    int ret = 0;
    while(path[it] != '\0'){
        if(path[it] == '/') ret++;
        it++;
    }
    return ret;
}

int depth_tree_structure(char **dirs){
    int it = 0;
    char *dir = dirs[it];
    int ret = depth_dir(dir);
    int tmp = 0;
    while(dir != NULL){
        tmp = depth_dir(dir);
        it++;
        dir = dirs[it];
        if(tmp > ret) ret = tmp;
    }
    return ret;
}

