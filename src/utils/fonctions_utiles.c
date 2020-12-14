#include "fonctions_utiles.h"

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
		strcat((*ad_wd).c_htar, "/");
		while(strstr(token, ".tar") == NULL)
		{
			strcat((*ad_wd).c_htar, token);
			strcat((*ad_wd).c_htar, "/");

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

	if(path_wd[strlen(path_wd)-1] != '/'){
		if(strlen((*ad_wd).c_tar) > 0){
			(*ad_wd).c_tar[strlen((*ad_wd).c_tar)-1] = '\0';
		}
		else if(strlen((*ad_wd).tar_name) == 0){
			(*ad_wd).c_htar[strlen((*ad_wd).c_htar)-1] = '\0';
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
		strcat(path_wd, wd.tar_name);

		if(strlen(wd.c_tar) > 0)
		{
			/* ajoute le chemin dans le tar à la chaine de caractères si il existe */
			strcat(path_wd, "/");
			strncat(path_wd, wd.c_tar, strlen(wd.c_tar));
		}
	}
}

/* cette fonction permet de rendre u_wd absolu en sachant que l'on est en cwd pour le moment */
void chemin_absolu(char * cwd, char * u_wd, char * u_wd_abs)
{
	int i, j;

	/* rend u_wd en chemin complet (part de la racine) */
	if(u_wd[0] != '/')
	{
		sprintf(u_wd_abs, "%s", cwd);
		if(cwd[strlen(cwd)-1] != '/')
		{
			strcat(u_wd_abs, "/");
		}
		strcat(u_wd_abs, u_wd);
	}
	else
	{
		sprintf(u_wd_abs, "%s", u_wd);
	}

	int nb_slash = 0;
	for(i = 0; i < strlen(u_wd_abs)-1; i++)
	{
		nb_slash += (u_wd_abs[i] == '/') ? 1 : 0;
	}


	char liste_rep[nb_slash][50];
	int ind_rep[nb_slash];

	/* retire les .. et . du chemin */
	char * u_wd_copy = strdup(u_wd_abs);
	char * token = strtok(u_wd_copy, "/");
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

	sprintf(u_wd_abs, "%s", "");
	for(i = 0; i < nb_slash; i++)
	{
		if(ind_rep[i])
		{
			strcat(u_wd_abs, "/");
			strcat(u_wd_abs, liste_rep[i]);
		}
	}

	strcat(u_wd_abs, "/");
}
