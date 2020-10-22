#include "list_file.h"
#include "types/posix_header.h"

int ls(int fd_out, char option, const char * path_tar, const char * path_cwd)
{
	/*
	- !! REMARQUE IMPORTANTE !! on considère que les path fournis existent
	- ls(...) : renvoie le nombre de fichiers si tout s'est bien passé et -1 sinon
	- fd_out : sortie pour affichage (STDOUT_FILENO pour les tests) ce sera le fd du tshell
	- option : ' ' pour un ls et 'l' pour un ls -l
	- path_tar : chemin du tar (exemple "chemin/.../test.tar")
	- path_cwd : chemin de la localisation de l'utilisateur (cwd) à partir du tar avec un '/' à la fin (exemple "dos1/dos2/dos3/") ou "" si il est à la racine du tar
	*/

	struct posix_header p;

	char buf[BLOCK_SIZE];
	//char affichage[BLOCK_SIZE] = "";

	int total = 0;
	int n, i, shift;

	int size_psize = sizeof(p.size);
	int size_dec;
	char size_str[size_psize];

	int size_pname = sizeof(p.name);
	char name[size_pname];


	char type[1];

	char time_str[16];

	char perm_str[sizeof(p.mode)];


	if(option != 'l' && option != ' ')
	{
		return -1;
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
			/* Fin du tar */
			close(fd);

			if(total > 0 && option != 'l')
			{
				n = write(fd_out, "\n", 1);
				if(n < 0)
				{
					perror("write 2");
					return -1;
				}
			}

			return total;
		}

		/* Récupère le nom complet du fichier */
		for (i = 0; i < size_pname; i++)
		{
			p.name[i] = buf[i];
		}

		/* Récupère la taille du fichier + conversion */
		for (i = 0; i < size_psize; i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec); //conversion str octal -> int décimal


		/* Vérification + conversion (à gauche) que le fichier est à afficher */
		if(verif_convert_name(name, p.name, path_cwd) != 0)
		{
			/* Ici name a son nom exact */
			char affichage[BLOCK_SIZE] = ""; //permet d'afficher fichier par fichier (on ne se pose plus la question du nombre de fichiers à afficher)
			if(option == 'l')
			{
				/* récupère le type */
				p.typeflag = buf[156];
				get_type(p.typeflag, type);
				strcat(affichage, type);

				/* récupère les permissions + ajout */
				for (i = 0; i < sizeof(p.mode); i++)
				{
					p.mode[i] = buf[100+i];
				}
				get_permissions(p.mode, perm_str);
				strcat(affichage, perm_str);
				strcat(affichage, "  ");

				/* récupère le uname + ajout */
				for (i = 0; i < sizeof(p.uname); i++)
				{
					p.uname[i] = buf[265+i];
				}
				strcat(affichage, p.uname);
				strcat(affichage, " ");

				/* récupère le gname + ajout */
				for (i = 0; i < sizeof(p.gname); i++)
				{
					p.gname[i] = buf[297+i];
				}
				strcat(affichage, p.gname);
				strcat(affichage, "  ");

				/* ajoute taille à message */
				sprintf(size_str, "%i", size_dec); //conversion int -> str
				strcat(affichage, size_str);
				strcat(affichage, "\t");

				/* récupère la date de dernière modification + ajout */
				for (i = 0; i < sizeof(p.mtime); i++)
				{
					p.mtime[i] = buf[136+i];
				}
				get_time(p.mtime, time_str);
				strcat(affichage, time_str);
				strcat(affichage, "  ");

				/* ajout nom à message */
				strcat(affichage, name);
				strcat(affichage,"\n");
			}
			else
			{
				/* ajoute le nom exact du fichier+"\t" au message à renvoyer */
				strcat(affichage, name);
				strcat(affichage, "\t");
			}

			n = write(fd_out, affichage, sizeof(affichage)); //plusieurs appels de write mais fonctionne à chaque fois (pas de depassement de la taille du buffer)
			if(n < 0)
			{
				perror("write 1");
				return -1;
			}

			total++;
		}


		/* Passe à l'en-tête suivant */
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
