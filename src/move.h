#ifndef MOVE
#define MOVE

#include "insert.h"
#include "types/posix_header.h"
#include "utils.h"
#include "insert.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, const char *path_dest);

#endif