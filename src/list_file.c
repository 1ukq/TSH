#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tar.h"



const char * tarName = "test.tar";
const char * chemin = "dos1/"; //doit etre dans un dossier ou à l'origine


int ls(int);


int main(void)
{
	int fd = open(tarName, O_RDONLY);
	if (fd < 0)
	{
		perror("open tarball");
		return -1;
	}

	ls(fd);

	close(fd);

	return 0;
}


int ls(int fd)
{
	struct posix_header p;
	int n;
	char message[500] = ""; //voir quelle valeur mettre.. ou chaine de taille variable??
	char buf[BLOCKSIZE];

	while((n = read(fd, buf, BLOCKSIZE)) > 0)
	{
		if (*(buf) == '\0')
		{
			//end of file
			strcat(message,"\n");
			n = write(STDOUT_FILENO, message, 500);
			if (n < 0)
			{
				perror("write");
				return -1;
			}
			return 0;
		}

		for (int i = 0; i < sizeof(p.name); i++)
		{
			*(p.name + i) = *(buf + i);
		}

		for (int i = 0; i < sizeof(p.size); i++)
		{
			*(p.size + i) = *(buf + 124 + i);
			//124 correspond à la position du début de size (voir tar.h)
		}

		//regarde si chemin est au debut du nom
		if(strncmp(p.name, chemin, strlen(chemin)) == 0)
		{
			//exclu chemin = position actuelle de l'affichage
			if (strcmp(p.name, chemin) != 0)
			{
				//exclure les fichiers de profondeur > |chemin| + 1
				strcat(strcat(message,p.name),"  ");
			}
		}

		for (int i = 0; i < ((atoi(p.size) + BLOCKSIZE -1)/BLOCKSIZE); i++)
		{
			n = read(fd, buf, BLOCKSIZE);
			if (n < 0)
			{
				perror("read");
				return -1;
			}
		}
	}
	perror("read");
	return -1;
}



/*
//cette fonction remplacera le code de la ligne 71 à 79
char * getName(char * chemin, char * nom)
{
	//la fonction vérifie si nom est bien à renvoyer et renvoie le nom à afficher si c'est le cas et renvoie "" sinon.
	//chemin est du type ...doc1/doc2/...
	//nom est du type /chemin/nom...

	//verifie que nom est dans le dossier
	if (strncmp(nom, chemin, strlen(chemin)) == 0)
	{

	}


}
*/
