#include "list_file.h"



int verif_convert_name(char * name, char * fullname, const char * path_cwd)
{
	/*
	La fonction vérifie que le fichier est à afficher et si il l'est, il convertit le fullname en name exact. Renvoie false (0) si le fichier n'est pas à afficher, true (!= 0) sinon et ajoute le nom exact dans name
	- name : emplacement nouvelle variable name
	- fullname : p.name (nom à vérifier puis convertir)
	- path_cwd : path_cwd
	*/

	int i, j, size_name;
	int size_fullname = 100;
	int len_path_cwd = strlen(path_cwd);
	int verif = ((strcmp(path_cwd, fullname) != 0) && ((strncmp(path_cwd, fullname, (len_path_cwd - 1)) == 0) || (strcmp(path_cwd,"") == 0)));

	if(verif)
	{
		/* Partie gauche vérifiée */
		/* Conversion partie gauche */
		for(i = 0, j = len_path_cwd; j < size_fullname - len_path_cwd ; i++, j++)
		{
			name[i] = fullname[j];
			if(name[i] == '\0')
			{
				break;
			}
		}

		size_name = strlen(name);
		/* Conversion partie droite */
		strtok(name, "/");

		/* Vérification dans le return */
		return (size_name <=  strlen(name) + 1);
	}

	return 0;
}

void get_type(char ptypeflag, char * type)
{
	//char * ltype = "-7lcbdps";
	type[0] = LTYPE[atoi(&ptypeflag)];
	type[1] = '\0';
}

void oct_to_dec(char * oct, char * dec)
{
	int dec_int;

	sscanf(oct, "%o", &dec_int);
	sprintf(dec, "%i", dec_int);
}

void dec_to_bin(char * dec, char * bin)
{
	int i;
	int nb = atoi(dec);

	for(i = 8; nb > 0; i--)
	{
		bin[i] = (nb%2) ? '1' : '0';
		nb = nb/2;
	}
}

void oct_to_bin(char * oct, char * bin)
{
    char dec_str[sizeof(oct)];

	oct_to_dec(oct, dec_str);
	dec_to_bin(dec_str, bin);
}

void get_permissions(char * perm_oct, char * perm_str)
{
	//char * lperm = "rwxrwxrwx";
	char perm_bin[10];
	oct_to_bin(perm_oct, perm_bin);

	for(int i = 0; i < 9; i++)
	{
		if(perm_bin[i] == '1')
		{
			perm_str[i] = LPERM[i];
		}
		else
		{
			perm_str[i] = '-';
		}
	}

	perm_str[sizeof(perm_str)+1] = '\0';
}

void get_time(char * time_oct, char * time)
{
	int i;
	int time_dec;
	sscanf(time_oct, "%o", &time_dec);

	time_t time_dec_t = time_dec;
	struct tm * t = localtime(&time_dec_t);

	char year[4];
	char month[2];
	char day[2];
	char hour[2];
	char min[2];

	sprintf(year, "%i", 1900+(*t).tm_year);
	sprintf(month, "%i", 1+(*t).tm_mon);
	sprintf(day, "%i", (*t).tm_mday);
	sprintf(hour, "%i", (*t).tm_hour);
	sprintf(min, "%i", (*t).tm_min);

	if(strlen(month) == 1)
	{
		month[1] = month[0];
		month[0] = '0';
	}

	if(strlen(day) == 1)
	{
		day[1] = day[0];
		day[0] = '0';
	}

	if(strlen(hour) == 1)
	{
		hour[1] = hour[0];
		hour[0] = '0';
	}

	if(strlen(min) == 1)
	{
		min[1] = min[0];
		min[0] = '0';
	}


	for(i = 0; i < 4; i++)
	{
		time[i] = year[i];
	}

	for(i = 0; i < 2; i++)
	{
		time[i+5] = month[i];
		time[i+8] = day[i];
		time[i+11] = hour[i];
		time[i+14] = min[i];
	}
	time[4] = '-';
	time[7] = '-';
	time[10] = ' ';
	time[13] = ':';
	time[16] = '\0';

}



int ls(char option, char * path, char * cwd)
{
	/*
	- !! REMARQUE IMPORTANTE !! on considère que les path fournis existent (en effet ils seront "filtrés" par la fonction cd qui vérifiera à chaque changement de cwd que le nouveau chemin existe)
	- ls(...) : renvoie le nombre de fichiers si tout s'est bien passé et -1 sinon
	- option : ' ' pour un ls et 'l' pour un ls -l
	- path : chemin absolu (impliquant un unique tar)

	return value:
	- (>= 0) nb fichiers si tout se passe bien
	- -1 si erreur de type write...
	- -2 si le chemin n'existe pas
	*/

	//Note : la fonction ne prend pas encore en compte le nombre de liens pour chaque fichiers
	struct work_directory wd;

	char path_in_tar[sizeof(wd.c_tar)];
	char path_to_tar[sizeof(wd.c_htar) + 1 + sizeof(wd.tar_name)];


	struct posix_header p;

	int total = 0;
	int n, shift;
	int path_exist = 0;

	int size_psize = sizeof(p.size);
	int size_dec;
	char size_str[size_psize];

	int size_pname = sizeof(p.name);
	char name[size_pname];

	char type[1];
	char time_str[17];
	char uid[8];
	char gid[8];
	char perm_str[sizeof(p.mode)];

	char full_path[strlen(path) + strlen(cwd) + 1];


	if(option != 'l' && option != ' ')
	{
		return -1;
	}

	chemin_absolu(cwd, path, full_path);
	fill_wd(full_path, &wd);
	if(strlen(wd.tar_name) == 0)
	{
		//le chemin n'implique aucun tar
		return -1;
	}

	sprintf(path_in_tar, "%s", wd.c_tar);
	sprintf(path_to_tar, "%s/%s", wd.c_htar, wd.tar_name);


	int fd = open(path_to_tar, O_RDONLY);
	if(fd < 0)
	{
		//le chemin n'existe pas
		return -2;
	}

	while(read(fd, &p, BLOCK_SIZE ) > 0)
	{
		if(p.name[0] == '\0')
		{
			/* Fin du tar */
			close(fd);

			if((total > 0) && (option != 'l'))
			{
				n = write(STDOUT_FILENO, "\n", 1);
				if(n < 0)
				{
					perror("write 2");
					return -1;
				}
			}

			if(path_exist == 0)
			{
				//le chemin n'existe pas
				return -2;
			}

			return total;
		}

		if(path_exist == 0)
		{
			if(strlen(path_in_tar) == 0)
			{
				path_exist = 1;
			}

			if(strcmp(p.name, path_in_tar) == 0)
			{
				path_exist = 1;
			}
		}

		/* Vérification + conversion (à gauche) que le fichier est à afficher */
		if(verif_convert_name(name, p.name, path_in_tar) != 0)
		{
			/* Ici name a son nom exact */
			char affichage[BLOCK_SIZE] = ""; //permet d'afficher fichier par fichier (on ne se pose plus la question du nombre de fichiers à afficher)
			if(option == 'l')
			{
				/* récupère le type */
				get_type(p.typeflag, type);
				strcat(affichage, type);

				/* ajout de permissions à affichage */
				get_permissions(p.mode, perm_str);
				strcat(affichage, perm_str);
				strcat(affichage, "  ");

				/* devrait récupérer le nombre de liens + ajout (ou le uid si le uname est vide) */

				/* ajout de uname ou uid à affichage */
				if(strlen(p.uname) > 0)
				{
					strcat(affichage, p.uname);
				}
				else
				{
					oct_to_dec(p.uid, uid);
					strcat(affichage, uid);
				}
				strcat(affichage, "/");

				/* ajout de gname ou gid à affichage */
				if(strlen(p.gname) > 0)
				{
					strcat(affichage, p.gname);
				}
				else
				{
					oct_to_dec(p.gid, gid);
					strcat(affichage, gid);
				}
				strcat(affichage, "  ");

				/* ajout de taille à affichage */
				sprintf(size_str, "%i", size_dec); //conversion int -> str
				strcat(affichage, size_str);
				strcat(affichage, "  ");

				/* ajout de time à affichage */
				get_time(p.mtime, time_str);
				strcat(affichage, time_str);
				strcat(affichage, "  ");

				/* ajout nom à affichage */
				strcat(affichage, name);
				strcat(affichage,"\n");
			}
			else
			{
				/* ajoute le nom exact du fichier+"\t" au message à renvoyer */
				strcat(affichage, name);
				strcat(affichage, "\t");
			}

			n = write(STDOUT_FILENO, affichage, strlen(affichage)); //plusieurs appels de write mais fonctionne à chaque fois (pas de depassement de la taille du buffer)
			if(n < 0)
			{
				perror("write 1");
				return -1;
			}

			total++;
		}


		/* Passe à l'en-tête suivant */
		sscanf(p.size, "%o", &size_dec); //conversion str octal -> int décimal
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
