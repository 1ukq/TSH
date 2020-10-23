#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open
#include <unistd.h> //read write close
#include <string.h> //strlen strcat...
#include <stdio.h> //perror
#include <stdlib.h> //atoi
#include <time.h> //gmtime



#ifndef LIST_FILE
#define LIST_FILE
#define BLOCK_SIZE 512
#define LTYPE "-7lcbdps"
#define LPERM "rwxrwxrwx"

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
	char * dec_str;

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
	sprintf(month, "%i", (*t).tm_mon);
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

int ls(int fd_out, char option, const char * path_tar, const char * path_loc);

#endif
