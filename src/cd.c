#include "cd.h"

/* cette fonction permet de remplir une structure work_directory à partir d'une chaine représentant un chemin */
void fill_wd(char * path_wd, struct work_directory * ad_wd)
{
	char * path_wd_copy = strdup(path_wd);
	char * token = strtok(path_wd_copy, "/");

	sprintf((*ad_wd).c_htar, "%s", "");
	sprintf((*ad_wd).tar_name, "%s", "");
	sprintf((*ad_wd).c_tar, "%s", "");

	if(token == NULL)
	{
		/* alors path_wd = "/" */
		strcat((*ad_wd).c_htar, "/");
	}
	else
	{
		/* regarde si le chemin implique un tar et ajoute le chemin hors tar dans c_htar */
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

		/* si le chemin implique un tar */
		if(token != NULL)
		{
			/* ajout du nom du tar dans tar_name */
			strcat((*ad_wd).tar_name, token);

			token = strtok(NULL, "/");

			/* si le chemin dans le tar n'est pas nul */
			while(token != NULL)
			{
				/* ajoute le chemin dans le tar dans c_tar */
				strcat((*ad_wd).c_tar, token);
				strcat((*ad_wd).c_tar, "/");

				token = strtok(NULL, "/");
			}
		}
	}
}

/* cette fonction permet de récupérer un chemin dans une chaîne de caractères à partir d'un chemin dans la structure work_directory */
void get_wd(struct work_directory wd, char * path_wd)
{
	/* ajoute le chemin hors tar à la chaine de caractères */
	sprintf(path_wd, "%s", wd.c_htar);
	if(strlen(wd.tar_name) > 0)
	{
		/* ajoute le nom du tar à la chaine de caractères si il y en a un */
		strcat(path_wd, "/");
		strcat(path_wd, wd.tar_name);

		if(strlen(wd.c_tar) > 0)
		{
			/* ajoute le chemin dans le tar à la chaine de caractères si il existe */
			strcat(path_wd, "/");
			strncat(path_wd, wd.c_tar, strlen(wd.c_tar)-1);
		}
	}
}

void chemin_propre(char * cwd)
{
	int i, j;

	int nb_slash = 0;
	for(i = 0; i < strlen(cwd); i++)
	{
		nb_slash += (cwd[i] == '/') ? 1 : 0;
	}

	char liste_rep[nb_slash][50];
	int ind_rep[nb_slash];

	char * cwd_copy = strdup(cwd);
	char * token = strtok(cwd_copy, "/");
	i = 0;
	while(token != NULL && i < nb_slash)
	{
		sprintf(liste_rep[i], "%s", token);
		token = strtok(NULL, "/");
		i++;
	}

	for(i = 0; i < nb_slash; i++)
	{
		if(strcmp(liste_rep[i], ".") == 0)
		{
			ind_rep[i] = 0;
		}
		else if(strcmp(liste_rep[i], "..") == 0)
		{
			ind_rep[i] = 0;
			j = i - 1;
			while(ind_rep[j] != 1)
			{
				j--;
			}
			ind_rep[j] = 0;
		}
		else
		{
			ind_rep[i] = 1;
		}
	}

	sprintf(cwd, "%s", "");
	for(i = 0; i < nb_slash; i++)
	{
		if(ind_rep[i])
		{
			strcat(cwd, "/");
			strcat(cwd, liste_rep[i]);
		}
	}
}

/* cette fonction agit comme un cd (dans un tar et aussi hors d'un tar) elle change la variable path_cwd entrée en argument par la variable path_nwd si celle-ci est correcte, et ne change pas sinon. De plus le changement "officiel" pour la partie du chemin hors-tar se fait avec un chdir. La fonction renvoie 0 si tout s'est passé comme prévu */
int cd(char * path_cwd, char * path_nwd)
{
	/* si el nouveau chemin est vide */
	if(strlen(path_nwd) == 0)
	{
		return 0;
	}

	/* si le nouveau chemin n'est pas absolu : on le rend absolu */
	if(path_nwd[0] != '/')
	{
		char * end_nwd = strdup(path_nwd);
		sprintf(path_nwd, "%s/", path_cwd);
		strcat(path_nwd, end_nwd);
	}

	/* permet de rendre le nouveau chemin "propre" à savoir remplace les . et .. en gardant l'intégrité du chemin */
	chemin_propre(path_nwd);

	struct posix_header p;

	char buf[BLOCK_SIZE];
	int shift, n, i, size_dec;

	struct work_directory cwd;
	fill_wd(path_cwd, &cwd);

	struct work_directory nwd;
	fill_wd(path_nwd, &nwd);


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
