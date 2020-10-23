#include "mkdir.h"


int makedir(int fd_out, char * path_tar, char * path_cwd, char * dir_name)
{
	//faire attention au cas où dir_name est de la forme ".../nom_du_dir"

	struct posix_header p;
	memset(&p, '\0', BLOCK_SIZE);

	char buf[BLOCK_SIZE];

	char end_blocks[2*BLOCK_SIZE];
	memset(end_blocks, '\0', 2*BLOCK_SIZE);

	int i, n;
	int shift;

	int size_psize = sizeof(p.size);
	int size_dec;
	int size_pname = sizeof(p.name);

	/* initialisation du nom complet (pour vérifier que le répertoire n'est pas déja existant) */
	char fullname[size_pname];
	memset(fullname, '\0', size_pname);

	sprintf(fullname, "%s", path_cwd);
	strcat(fullname, dir_name);
	if(dir_name[strlen(dir_name)-1] != '/')
	{
		strcat(fullname, "/");
	}


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
			/* fin du tar*/
			/* p.name */
			sprintf(p.name, "%s", fullname);

			/* p.mode */
			sprintf(p.mode, "%s", "0000755");

			/* uid */
			sprintf(p.uid, "%07o", getuid());

			/* gid */
			sprintf(p.gid, "%07o", getgid());

			/* size */
			sprintf(p.size, "%011o", 0);

			/* mtime */
			sprintf(p.mtime, "%lo", time(NULL));

			/* typeflag */
			p.typeflag = '5';

			/* linkname */

			/* magic */
			sprintf(p.magic, "ustar");

			/* version */
			sprintf(p.version, "00");

			/* uname */
			struct passwd * pswd = getpwuid(getgid());
			sprintf(p.uname, "%s", (*pswd).pw_name);

			/* gname */
			struct group * grp = getgrgid(getgid());
			sprintf(p.gname, "%s", (*grp).gr_name);;

			/* devmajor */

			/* devminor */

			/* prefix */

			/* junk */

			/* chksum */
			set_checksum(&p);

			/* Place le curseur en dessous du contenu du dernier fichier */
			n = lseek(fd, (-1)*BLOCK_SIZE, SEEK_CUR);
			if(n < 0)
			{
				perror("lseek 2");
				return -1;
			}
			/* Écrit l'entête du nouveau répertoire */
			n = write(fd, &p, BLOCK_SIZE);
			if(n < 0)
			{
				perror("write 1");
				return -1;
			}
			/* Écrit les 2 blocs de fin du tar */
			n = write(fd, end_blocks, 2*BLOCK_SIZE);
			if(n < 0)
			{
				perror("write 2");
				return -1;
			}

			close(fd);

			return 0;
		}

		/* Regarde si le répertoire n'est pas déjà existant */
		if(strcmp(fullname, p.name) == 0)
		{
			// le répertoire existe deja
			close(fd);
			n = write(fd_out, "mkdir: cannot create directory : File already exists\n", 53);
			if(n < 0)
			{
				perror("write 3");
				return -1;
			}

			return 0;
		}
		/* Récupère la taille du fichier + conversion */
		for (i = 0; i < size_pname; i++)
		{
			p.name[i] = buf[i];
		}

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
