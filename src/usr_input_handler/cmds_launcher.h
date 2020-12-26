#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include "../utils/find_file.h"
#include "redirection.h"
#include "../utils/fonctions_utiles.h"

#define NO_RED 0

#define RED_OUT_APPEND_OUT_TAR 1 // >>
#define RED_OUT_TRUNC_OUT_TAR 2 // >
#define RED_ERR_APPEND_OUT_TAR 3 // 2>>
#define RED_ERR_TRUNC_OUT_TAR 4 // 2>
#define RED_IN_FROM_FILE 5 // <

#define RED_OUT_APPEND_IN_TAR 6
#define RED_OUT_TRUNC_IN_TAR 7
#define RED_ERR_APPEND_IN_TAR 8
#define RED_ERR_TRUNC_IN_TAR 9
#define RED_IN_FROM_TAR 10

int cmds_launcher(char ***cmds, int red_type, char *file);

void split_path(char * path, char * path_to_tar, char * path_in_tar);
