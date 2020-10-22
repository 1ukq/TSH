#include "mkdir.h"


int makedir(char * path_tar, char * path_cwd, char * dir_name)
{
	//il faut vérfier que dir_name n'existe pas deja en cwd !
	struct posix_header p;
	char buf[BLOCK_SIZE];
	char entete[BLOCK_SIZE];

	int i, n;
	int shift;

	int size_psize = sizeof(p.size);
	int size_dec;




	int fd = open(path_tar, O_RDWR);
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
			for(i = strlen(p.name); i < sizeof(p.name); i++)
			{
				p.name[i] = '\0';
			}
			printf("p.name : %s\n", p.name);

			/* p.mode */
			sprintf(p.mode, "%s", "000755");
			p.mode[sizeof(p.mode)-2] = ' ';
			p.mode[sizeof(p.mode)-1] = '\0';
			printf("p.mode : %s\n", p.mode);

			/* uid */
			sprintf(p.uid, "%6o", getuid());
			for(i = 0; i < sizeof(p.uid)-1; i++)
			{
				if(p.uid[i] == ' ')
				{
					p.uid[i] = '0';
				}
			}
			p.uid[sizeof(p.uid)-2] = ' ';
			p.uid[sizeof(p.uid)-1] = '\0';
			printf("p.uid : %s\n", p.uid);

			/* gid */
			sprintf(p.gid, "%6o", getgid());
			for(i = 0; i < sizeof(p.gid); i++)
			{
				if(p.gid[i] == ' ')
				{
					p.gid[i] = '0';
				}
			}
			p.gid[sizeof(p.gid)-2] = ' ';
			p.gid[sizeof(p.gid)-1] = '\0';
			printf("p.gid : %s\n", p.gid);

			/* size */
			for(i = 0; i < sizeof(p.size) - 1; i++)
			{
				p.size[i] = '0';
			}
			p.size[sizeof(p.size)-1] = ' ';
			printf("p.size : %s\n", p.size);

			/* mtime */
			sprintf(p.mtime, "%lo", time(NULL));
			p.mtime[sizeof(p.mtime)-1] = ' ';
			printf("p.mtime : %s\n", p.mtime);

			/* chksum */
			set_checksum(&p);
			printf("p.chksum : %s\n", p.chksum);

			/* typeflag */
			p.typeflag = '5';
			printf("p.typeflag : %c\n", p.typeflag);

			/* linkname */
			for(i = 0; i < sizeof(p.linkname); i++)
			{
				p.linkname[i] = '\0';
			}
			printf("p.linkname : %s\n", p.linkname);

			/* magic */
			sprintf(p.magic, "ustar");
			printf("p.magic : %s\n", p.magic);

			/* version */
			sprintf(p.version, "00");
			printf("p.version : %s\n", p.version);

			/* uname */
			struct passwd * pswd = getpwuid(getgid());
			sprintf(p.uname, "%s", (*pswd).pw_name);
			for(i = strlen(p.uname); i < sizeof(p.uname); i++)
			{
				p.uname[i] = '\0';
			}
			printf("p.uname : %s\n", p.uname);

			/* gname */
			struct group * grp = getgrgid(getgid());
			sprintf(p.gname, "%s", (*grp).gr_name);
			for(i = strlen(p.gname); i < sizeof(p.gname); i++)
			{
				p.gname[i] = '\0';
			}
			printf("p.gname : %s\n", p.gname);

			/* devmajor */
			for(i = 0; i < 6; i++)
			{
				p.devmajor[i] = '0';
			}
			p.devmajor[sizeof(p.devmajor)-2] = ' ';
			p.devmajor[sizeof(p.devmajor)-1] = '\0';
			printf("p.devmajor : %s\n", p.devmajor);

			/* devminor */
			for(i = 0; i < 6; i++)
			{
				p.devminor[i] = '0';
			}
			p.devminor[sizeof(p.devminor)-2] = ' ';
			p.devminor[sizeof(p.devminor)-1] = '\0';
			printf("p.devminor : %s\n", p.devminor);

			/* prefix */
			for(i = 0; i < sizeof(p.prefix); i++)
			{
				p.prefix[i] = '\0';
			}
			printf("p.prefix : %s\n", p.prefix);

			/* junk */
			for(i = 0; i < sizeof(p.junk); i++)
			{
				p.junk[i] = '\0';
			}
			printf("p.junk : %s\n", p.junk);


			// puis on ecrit tout dans l'ordre et la taille indiqué dans le posix_header en completant avec des '\0'

			n = lseek(fd, (-1)*BLOCK_SIZE, SEEK_CUR);
			printf("%i\n", SEEK_CUR);
			if(n < 0)
			{
				perror("lseek 2");
				return -1;
			}
			n = write(fd, &p, BLOCK_SIZE);
			if(n < 0)
			{
				perror("write");
				return -1;
			}

			char end_block[BLOCK_SIZE];
			memset(end_block, '\0', BLOCK_SIZE);

			n = write(fd, end_block, BLOCK_SIZE);
			n = write(fd, end_block, BLOCK_SIZE);

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
			perror("lseek 1");
			return -1;
		}
	}

	perror("read");
	return -1;

}
