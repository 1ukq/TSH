#ifndef PIPE
#define PIPE

#include <unistd.h>
#include <stdio.h>

int pipe_tsh(const char *cmd1, const char *opt1, const char *cmd2, const char *opt2);

#endif