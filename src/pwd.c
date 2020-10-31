#include "pwd.h"

int pwd(char * path_cwd, int fd_out)
{
	int n;
	char message[strlen(path_cwd) + 1];

	sprintf(message, "%s", path_cwd);
	strcat(message, "\n");

	n = write(fd_out, message, strlen(message));
	if(n < 0)
	{
		perror("write");
		return -1;
	}

	return 0;
}
