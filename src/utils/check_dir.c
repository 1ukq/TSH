#include "check_dir.h"

int contains_entry(const char *path_dir, const char *entry_name){

    DIR *dir;
    dir = opendir(path_dir);
    if(dir == NULL){
        return -1;
    }

    struct dirent *entry;
    entry = readdir(dir);

    while(entry != NULL){

        if(strcmp((*entry).d_name, entry_name) == 0){
            return 0;
        }
        entry = readdir(dir);

    }

    return 1;

}