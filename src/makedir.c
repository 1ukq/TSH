#include "makedir.h"

void set_checksum_mkdir(struct posix_header *hd){

  memset(hd -> chksum, ' ', 8);
  unsigned int sum = 0;
  char *p = (char *)hd;
  for (int i=0; i < BLOCK_SIZE; i++){
		sum += p[i];
	}
  sprintf(hd -> chksum, "%06o", sum);
}

void get_prev_rep(char * path, char * path_prev_rep){

	int len = strlen(path);
	int i = len;
	int nb_slash = 0;

	sprintf(path_prev_rep, "%s", "");

	while(i >= 0 && nb_slash < 2){
		if(path[i] == '/'){
			nb_slash++;
		}
		i--;
	}

	if(i > 0){
		char * path_tempo = strndup(path, i+2);
		sprintf(path_prev_rep,"%s", path_tempo);
	}
}


//int makedir(char * path_tar, char * path_cwd, char * dir_name)
int makedir(char * path)
{
	struct work_directory wd;

	char path_in_tar[sizeof(wd.c_tar)];
	char path_to_tar[sizeof(wd.c_htar) + 1 + sizeof(wd.tar_name)];

	struct posix_header p;
	memset(&p, '\0', BLOCK_SIZE);

	char buf[BLOCK_SIZE];

	char end_blocks[2*BLOCK_SIZE];
	memset(end_blocks, '\0', 2*BLOCK_SIZE);

	int i, n;
	int shift;
	int prev_rep_exist = 0;

	int size_psize = sizeof(p.size);
	int size_dec;
	int size_pname = sizeof(p.name);

	fill_wd(path, &wd);
	if(strlen(wd.tar_name) == 0 || strlen(wd.c_tar) == 0)
	{
		//le chemin n'implique aucun tar (chemin invalide)
		return -2;
	}

	sprintf(path_to_tar, "%s/%s", wd.c_htar, wd.tar_name);
	sprintf(path_in_tar, "%s", wd.c_tar);

	char path_prev_rep[strlen(path_in_tar)];
	get_prev_rep(path_in_tar, path_prev_rep);


	int fd = open(path_to_tar, O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}



	while(read(fd, buf, BLOCK_SIZE ) > 0)
	{
		if((buf[0] == '\0'))
		{
			/* fin du tar*/

			if(prev_rep_exist == 0){
				close(fd);
				//le sous-répertoire n'existe pas (chemin invalide)
				return -2;
			}

			/* p.name */
			sprintf(p.name, "%s", path_in_tar);

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
			set_checksum_mkdir(&p);

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

		/* Récupère le nom du fichier */
		for (i = 0; i < size_pname; i++)
		{
			p.name[i] = buf[i];
		}

		/* Regarde si le répertoire n'est pas déjà existant */
		if(strcmp(path_in_tar, p.name) == 0)
		{
			// le répertoire existe deja
			close(fd);

			return -3;
		}

		/* Regarde si le sous-répertoire existe */
		if(prev_rep_exist == 0){
			if(strlen(path_prev_rep) == 0 || strcmp(path_prev_rep, p.name) == 0){
				prev_rep_exist = 1;
			}
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
