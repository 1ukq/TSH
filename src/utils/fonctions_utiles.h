#ifndef FONCTIONS_UTILES
#define FONCTIONS_UTILES

#include <string.h>
#include <stdio.h>
#include "../types/work_directory.h"

void fill_wd(char * path_wd, struct work_directory * ad_wd);

void get_wd(struct work_directory wd, char * path_wd);

void chemin_absolu(char * cwd, char * u_wd, char * u_wd_abs);

#endif
