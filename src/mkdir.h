#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include "types/posix_header.h"


#define BLOCK_SIZE 512


void set_checksum(struct posix_header *hd)
{
  	memset(hd -> chksum, ' ', 8);
  	unsigned int sum = 0;
  	char *p = (char *)hd;
  	for (int i=0; i < BLOCK_SIZE; i++)
  	{
		sum += p[i];
	}
  	sprintf(hd -> chksum, "%06o", sum);
}
