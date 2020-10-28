#include "cd.h"




struct work_directory
{
  	char cwd_ht[100];	//cwd hors-tar "/home/.../dossier"
	char tar_name[100]; //nom du tar "tar_name"
  	char cwd_t[100];	//cwd tar "dos1/dos2/.../dosk/"
};




void get_cwd(char * path_cwd, struct work_directory wd)
{
	sprintf(path_cwd, "%s", wd.cwd_ht);
	if(strlen(wd.tar_name) > 0)
	{
		strcat(path_cwd, "/");
		strcat(path_cwd, wd.tar_name);

		if(strlen(wd.cwd_t)>0)
		{
			strcat(path_cwd, "/");
			strncat(path_cwd, wd.cwd_t, strlen(wd.cwd_t)-1);
		}
	}
}




int get_path_tar(char * path_tar, struct work_directory wd)
{
	int len_tar_name = strlen(wd.tar_name);

	if(len_tar_name > 0)
	{
		sprintf(path_tar, "%s", wd.cwd_ht);
		strcat(path_tar, "/");
		strcat(path_tar, wd.tar_name);

		return len_tar_name;
	}

	return 0;
}




int cd(struct work_directory * ad_cwd, struct work_directory * ad_nwd)
{
	char path_tar[sizeof((*ad_nwd).cwd_ht) + sizeof((*ad_nwd).cwd_t)];
	get_path_tar(path_tar, *ad_nwd);

	struct posix_header p;

	char buf[BLOCK_SIZE];

	int shift, n, i, size_dec;





	//vérifier que nwd.cwd_ht existe






	if(strlen((*ad_nwd).tar_name) <= 0)
	{
		*ad_cwd = *ad_nwd;



		//faire un "vrai" cd cwd.cwd_ht




		return 0;
	}




	//savoir si le tar existe à l'emplacement indiqué





	if(strlen((*ad_nwd).cwd_t) <= 0)
	{
		//on est sûr que nwd existe
		*ad_cwd = *ad_nwd;




		//faire un "vrai" cd cwd.cwd_ht




		return 0;
	}


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
			//end of tar_file
			close(fd);
			//le nwd n'existe pas
			//cwd reste inchangé
			return 0;
		}

		/* Récupère la taille du fichier + conversion */
		for (i = 0; i < sizeof(p.size); i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec);

		/* récupère le type */
		p.typeflag = buf[156];

		if(p.typeflag == '5') //dossier
		{
			/* Récupère le nom complet du fichier */
			for (i = 0; i < sizeof(p.name); i++)
			{
				p.name[i] = buf[i];
			}


			if(strcmp((*ad_nwd).cwd_t, p.name) == 0)
			{
				//nwd existe
				//nwd devient cwd
				*ad_cwd = *ad_nwd;




				//faire un "vrai" cd cwd.cwd_ht




				return 0;
			}
		}


		shift = (size_dec + BLOCK_SIZE -1)/BLOCK_SIZE;
		n = lseek(fd, shift*BLOCK_SIZE, SEEK_CUR);
		if(n < 0)
		{
			perror("lseek");
			return -1;
		}


	}

	close(fd);

	printf("%s\n", path_tar);
}
