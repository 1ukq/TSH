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
			for(i = 0; i < sizeof(p.name); i++)
			{
				entete[i] = p.name[i];
			}
			printf("p.name : %s\n", p.name);

			/* p.mode */
			sprintf(p.mode, "%s", "0000755");
			for(i = 0; i < sizeof(p.mode); i++)
			{
				entete[100 + i] = p.mode[i];
			}
			printf("p.mode : %s\n", p.mode);

			/* uid */
			sprintf(p.uid, "%7o", getuid());
			for(i = 0; i < sizeof(p.uid)-1; i++)
			{
				if(p.uid[i] == ' ')
				{
					p.uid[i] = '0';
				}
			}
			p.uid[sizeof(p.uid)-1] = '\0';
			for(i = 0; i < sizeof(p.uid); i++)
			{
				entete[108 + i] = p.uid[i];
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
			p.gid[sizeof(p.gid)-1] = '\0';
			for(i = 0; i < sizeof(p.gid); i++)
			{
				entete[116 + i] = p.gid[i];
			}
			printf("p.gid : %s\n", p.gid);

			/* size */
			for(i = 0; i < sizeof(p.size) - 1; i++)
			{
				p.size[i] = '0';
			}
			p.size[sizeof(p.size)-1] = '\0';
			for(i = 0; i < sizeof(p.size); i++)
			{
				entete[124 + i] = p.size[i];
			}
			printf("p.size : %s\n", p.size);

			/* mtime */
			sprintf(p.mtime, "%li", time(NULL));
			for(i = 0; i < sizeof(p.mtime); i++)
			{
				entete[136 + i] = p.mtime[i];
			}
			printf("p.mtime : %s\n", p.mtime);

			/* chksum */
			set_checksum(&p);
			for(i = 0; i < sizeof(p.chksum); i++)
			{
				entete[148 + i] = p.chksum[i];
			}
			printf("p.chksum : %s\n", p.chksum);

			/* typeflag */
			p.typeflag = '5';
			entete[156] = p.typeflag;
			printf("p.typeflag : %c\n", p.typeflag);

			/* linkname */
			for(i = 0; i < sizeof(p.linkname); i++)
			{
				p.linkname[i] = '\0';
			}
			for(i = 0; i < sizeof(p.linkname); i++)
			{
				entete[157 + i] = p.linkname[i];
			}
			printf("p.linkname : %s\n", p.linkname);

			/* magic */
			sprintf(p.magic, "ustar");
			for(i = 0; i < sizeof(p.magic); i++)
			{
				entete[257 + i] = p.magic[i];
			}
			printf("p.magic : %s\n", p.magic);

			/* version */
			sprintf(p.version, "00");
			for(i = 0; i < sizeof(p.version); i++)
			{
				entete[263 + i] = p.version[i];
			}
			printf("p.version : %s\n", p.version);

			/* uname */
			struct passwd * pswd = getpwuid(getgid());
			sprintf(p.uname, "%s", (*pswd).pw_name);
			for(i = strlen(p.uname); i < sizeof(p.uname); i++)
			{
				p.uname[i] = '\0';
			}
			for(i = 0; i < sizeof(p.uname); i++)
			{
				entete[265 + i] = p.uname[i];
			}
			printf("p.uname : %s\n", p.uname);

			/* gname */
			struct group * grp = getgrgid(getgid());
			sprintf(p.gname, "%s", (*grp).gr_name);
			for(i = strlen(p.gname); i < sizeof(p.gname); i++)
			{
				p.gname[i] = '\0';
			}
			for(i = 0; i < sizeof(p.gname); i++)
			{
				entete[297 + i] = p.gname[i];
			}
			printf("p.gname : %s\n", p.gname);

			/* devmajor */
			for(i = 0; i < 6; i++)
			{
				p.devmajor[i] = '0';
			}
			p.devmajor[sizeof(p.devmajor)-2] = ' ';
			p.devmajor[sizeof(p.devmajor)-1] = '\0';
			for(i = 0; i < sizeof(p.devmajor); i++)
			{
				entete[329 + i] = p.devmajor[i];
			}
			printf("p.devmajor : %s\n", p.devmajor);

			/* devminor */
			for(i = 0; i < 6; i++)
			{
				p.devminor[i] = '0';
			}
			p.devminor[sizeof(p.devminor)-2] = ' ';
			p.devminor[sizeof(p.devminor)-1] = '\0';
			for(i = 0; i < sizeof(p.devminor); i++)
			{
				entete[337 + i] = p.devminor[i];
			}
			printf("p.devminor : %s\n", p.devminor);

			/* prefix */
			for(i = 0; i < sizeof(p.prefix); i++)
			{
				p.prefix[i] = '\0';
			}
			for(i = 0; i < sizeof(p.prefix); i++)
			{
				entete[345 + i] = p.prefix[i];
			}
			printf("p.prefix : %s\n", p.prefix);

			/* junk */
			for(i = 0; i < sizeof(p.junk); i++)
			{
				p.junk[i] = '\0';
			}
			for(i = 0; i < sizeof(p.junk); i++)
			{
				entete[500 + i] = p.junk[i];
			}
			printf("p.junk : %s\n", p.junk);


			// puis on ecrit tout dans l'ordre et la taille indiqué dans le posix_header en completant avec des '\0'

			n = lseek(fd, (-1)*BLOCK_SIZE, SEEK_CUR);
			if(n < 0)
			{
				perror("lseek 2");
				return -1;
			}
			n = write(fd, entete, BLOCK_SIZE);
			if(n < 0)
			{
				perror("write");
				return -1;
			}

			char end_block[BLOCK_SIZE];
			memset(end_block, '\0', BLOCK_SIZE);

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
