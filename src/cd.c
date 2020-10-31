#include "cd.h"

void fill_wd(char * path_wd, struct work_directory * ad_wd)
{
	char * path_wd_copy = strdup(path_wd);
	char * token = strtok(path_wd_copy, "/");

	sprintf((*ad_wd).c_htar, "%s", "");
	sprintf((*ad_wd).tar_name, "%s", "");
	sprintf((*ad_wd).c_tar, "%s", "");

	while(strstr(token, ".tar") == NULL)
	{
		strcat((*ad_wd).c_htar, "/");
		strcat((*ad_wd).c_htar, token);

		token = strtok(NULL, "/");

		if(token == NULL)
		{
			break;
		}
	}

	if(token != NULL)
	{
		strcat((*ad_wd).tar_name, token);

		token = strtok(NULL, "/");

		while(token != NULL)
		{
			strcat((*ad_wd).c_tar, token);
			strcat((*ad_wd).c_tar, "/");

			token = strtok(NULL, "/");
		}
	}
}

void get_wd(struct work_directory wd, char * path_wd)
{
	sprintf(path_wd, "%s", wd.c_htar);
	if(strlen(wd.tar_name) > 0)
	{
		strcat(path_wd, "/");
		strcat(path_wd, wd.tar_name);

		if(strlen(wd.c_tar)>0)
		{
			strcat(path_wd, "/");
			strncat(path_wd, wd.c_tar, strlen(wd.c_tar)-1);
		}
	}
}




int cd(char * path_cwd, char * path_nwd)
{
	struct posix_header p;

	char buf[BLOCK_SIZE];
	int shift, n, i, size_dec;

	struct work_directory cwd;
	fill_wd(path_cwd, &cwd);

	struct work_directory nwd;
	fill_wd(path_nwd, &nwd);


	/* si le nouveau "chemin" est "." */
	if(strcmp(path_nwd, ".") == 0)
	{
		/* on ne fait rien */
		return 0;
	}

	/* si le nouveau "chemin" est ".." */
	/*
	if(strcmp(path_nwd, "..") == 0)
	{
		if(strlen(nwd.c_tar) == 0)
		{
			if(strlen(nwd.tar_name) == 0)
			{
				if(strlen(nwd.c_htar) == 0)
				{

				}
				else
				{

				}
			}
			else
			{
				sprintf(nwd.tar_name, "");
				cwd = nwd;
				fill_wd(path_cwd, cwd);
			}
		}
		else
		{

		}

		return 0;
	}
	*/



	/* vérfie que le nouveau chemin hors-tar existe et change "vraiment" de directory */
	if(chdir(nwd.c_htar) < 0)
	{
		/* le nouveau chemin hors-tar n'existe pas; on ne change pas de cwd */
		return 0;
	}

	/* vérifie que le nouveau chemin implique un tar */
	if(strlen(nwd.tar_name) <= 0)
	{
		/* le nouveau chemin n'implique pas de tar; on est sûr que nwd existe; cwd devient nwd */
		cwd = nwd;
		get_wd(cwd, path_cwd);

		return 0;
	}

	/* vérifie que le tar existe à l'emplacement indiqué */
	int fd = open(nwd.tar_name, O_RDONLY);
	if(fd < 0)
	{
		/* le tar n'existe pas à l'endroit indiqué; le nwd n'existe donc pas; on reste dans l'ancien cwd -> on se remet dans le répertoire d'origine */
		chdir(cwd.c_htar);

		return 0;
	}

	/* vérifie que le nouveau chemin dans le tar est non-vide (sinon on est sûr que le chemin existe) */
	if(strlen(nwd.c_tar) <= 0)
	{
		/* le nouveau chemin dans le tar est vide; on sûr que nwd existe; cwd devient nwd */
		cwd = nwd;
		get_wd(cwd, path_cwd);

		return 0;
	}

	/* lit en boucle les noms des repertoires dans le tar et vérifie qu'il en existe un de nom nwd.c_tar (= vérfie que le nouveau chemin dans le tar existe) */
	while(read(fd, buf, BLOCK_SIZE ) > 0)
	{
		if(buf[0] == '\0')
		{
			/* on arrive à la fin du tar => le nouveau chemin dans le tar n'existe pas; on se remet dans le répertoire d'origine; cwd reste inchangé */
			close(fd);

			chdir(cwd.c_htar);

			return 0;
		}

		/* Récupère la taille du fichier + conversion */
		for (i = 0; i < sizeof(p.size); i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec);

		/* Récupère le type */
		p.typeflag = buf[156];

		if(p.typeflag == '5') //répertoire
		{
			/* Récupère le nom complet du fichier */
			for (i = 0; i < sizeof(p.name); i++)
			{
				p.name[i] = buf[i];
			}


			if(strcmp(nwd.c_tar, p.name) == 0)
			{
				/* le nouveau chemin dans le tar existe => nwd existe; cwd devient nwd */
				cwd = nwd;
				get_wd(cwd, path_cwd);

				return 0;
			}
		}

		/* passe directement à l'en-tête suivant en sautant le contenu des fichier */
		shift = (size_dec + BLOCK_SIZE -1)/BLOCK_SIZE;
		n = lseek(fd, shift*BLOCK_SIZE, SEEK_CUR);
		if(n < 0)
		{
			perror("lseek");
			return -1;
		}


	}

	close(fd);

}
