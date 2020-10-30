#ifndef CD
#define CD

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "types/posix_header.h"
#include "types/work_directory.h"

#define BLOCK_SIZE 512


int cd(struct work_directory * ad_cwd, struct work_directory * ad_nwd);

#endif
