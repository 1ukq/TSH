#include "mkdir.h"
#include "types/posix_header.h"


int makedir(char * path_tar, char * path_cwd, char * dir_name)
{
	struct posix_header p;
	char buf[BLOCK_SIZE];

	int i, n;
	int shift;

	int size_psize = sizeof(p.size);
	int size_dec;

	char * mode = "0000755";




	int fd = open(path_tar, O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	while(read(fd, buf, BLOCK_SIZE ) > 0)
	{
		if(buf[0] == '\0')
		{
			/* Fin du tar */


			/* p.name */
			for(i = 0; i < strlen(path_cwd); i++)
			{
				p.name[i] = path_cwd[i];
			}
			strcat(p.name, dir_name);
			printf("%s\n", p.name);

			/* p.mode */
			for(i = 0; i < sizeof(p.mode); i++)
			{
				p.mode[i] = mode[i];
			}
			printf("%s\n", p.mode);

			/* uid */

			/* gid */

			/* size */

			/* mtime */

			/* chksum */

			/* typeflag */

			/* linkname */

			/* magic */

			/* version */

			/* uname */

			/* gname */

			/* devmajor */

			/* devminor */

			/* prefix */

			/* junk */


			// puis on ecrit tout dans l'ordre et la taille indiqué dans le posix_header en completant avec dse '\0'


			close(fd);

			return 0;
		}

		/* Récupère la taille du fichier + conversion */
		for (i = 0; i < size_psize; i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec); //conversion str octal -> int décimal

		/* Passe à l'en-tête suivant */
		shift = (size_dec + BLOCK_SIZE -1)/BLOCK_SIZE;
		n = lseek(fd, shift*BLOCK_SIZE, SEEK_CUR);
		if(n < 0)
		{
			perror("lseek");
			return -1;
		}
	}

	perror("read");
	return -1;

}
