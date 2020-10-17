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
			sprintf(p.name, "%s", path_cwd);
			strcat(p.name, dir_name);
			if(dir_name[strlen(dir_name)-1] != '/')
			{
				strcat(p.name, "/");
			}
			printf("p.name : %s\n", p.name);

			/* p.mode */
			sprintf(p.mode, "%s", "0000755");
			printf("p.mode : %s\n", p.mode);

			/* uid */
			sprintf(p.uid, "%7o", getuid());
			for(i = 0; i < sizeof(p.uid); i++)
			{
				if(p.uid[i] == ' ')
				{
					p.uid[i] = '0';
				}
			}
			printf("p.uid : %s\n", p.uid);

			/* gid */
			sprintf(p.gid, "%7o", getgid());
			for(i = 0; i < sizeof(p.gid); i++)
			{
				if(p.gid[i] == ' ')
				{
					p.gid[i] = '0';
				}
			}
			printf("p.gid : %s\n", p.gid);

			/* size */
			for(i = 0; i < sizeof(p.size); i++)
			{
				p.size[i] = '0';
			}
			p.size[sizeof(p.size)] = '\0';
			printf("p.size : %s\n", p.size);

			/* mtime */
			sprintf(p.mtime, "%li", time(NULL));
			printf("p.mtime : %s\n", p.mtime);

			/* chksum */
			printf("p.chksum : %s\n", p.chksum);

			/* typeflag */
			p.typeflag = '5';
			printf("p.typeflag : %c\n", p.typeflag);

			/* linkname */
			printf("p.linkname : %s\n", p.linkname);

			/* magic */
			printf("p.magic : %s\n", p.magic);

			/* version */
			printf("p.version : %s\n", p.version);

			/* uname */
			//sprintf(p.uname, "%s", getlogin());
			printf("p.uname : %s\n", p.uname);

			/* gname */
			printf("p.gname : %s\n", p.gname);

			/* devmajor */
			printf("p.devmajor : %s\n", p.devmajor);

			/* devminor */
			printf("p.devminor : %s\n", p.devminor);

			/* prefix */
			printf("p.prefix : %s\n", p.prefix);

			/* junk */
			printf("p.junk : %s\n", p.junk);


			// puis on ecrit tout dans l'ordre et la taille indiqué dans le posix_header en completant avec des '\0'


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
