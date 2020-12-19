#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "find_file.h"
#include "../redirection.h"

#define NO_RED 0

#define RED_OUT_APPEND_OUT_TAR 1
#define RED_OUT_TRUNC_OUT_TAR 2
#define RED_ERR_APPEND_OUT_TAR 3
#define RED_ERR_TRUNC_OUT_TAR 4
#define RED_IN_FROM_FILE 5

#define RED_OUT_APPEND_IN_TAR 6
#define RED_OUT_TRUNC_IN_TAR 7
#define RED_ERR_APPEND_IN_TAR 8
#define RED_ERR_TRUNC_IN_TAR 9
#define RED_IN_FROM_TAR 10

int cmds_launcher(char ***cmds, int red_type, char *file);