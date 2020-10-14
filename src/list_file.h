#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open
#include <unistd.h> //read write close
#include <string.h> //strlen strcat...
#include <stdio.h> //perror
#include <stdlib.h> //atoi



#ifndef LIST_FILE
#define LIST_FILE
#define BLOCK_SIZE 512

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
	char * ltype = "-7lcbdps";
	type[0] = ltype[atoi(&ptypeflag)];
	type[1] = '\0';
}

void dec_to_bin(char * dec, char * bin)
{

}

void get_permissions(char * perm_dec, char * perm_str)
{
	char * lperm = "rwxrwxrwx";
	char * perm_bin;
	dec_to_bin(perm_dec, perm_bin);

	for(int i = 0; i < 9; i++)
	{
		if(perm_bin[i] == '1')
		{
			perm_str[i] = lperm[i];
		}
		else
		{
			perm_str[i] = '-';
		}
	}
}

int ls(int fd_out, char option, const char * path_tar, const char * path_loc);

#endif
