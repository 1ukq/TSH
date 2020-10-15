#include "list_file.h"
#include "types/posix_header.h"

int ls(int fd_out, char option, const char * path_tar, const char * path_cwd)
{
	struct posix_header p;

	char buf[BLOCK_SIZE];
	char affichage[BLOCK_SIZE] = "";

	int total = 0;
	int n, i, j, shift;
	int len_path_cwd = strlen(path_cwd);

	int size_dec;
	char size_str[sizeof(p.size)];

	int size_pname = sizeof(p.name);
	char name[size_pname];

	char type[1];

	int time_dec;
	char time_str[sizeof(p.mode)];

	char perm_str[9];






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

			if (strlen(affichage) > 0)
			{
				if(option != 'l')
				{
					strcat(affichage, "\n");
				}

				n = write(fd_out, affichage, sizeof(affichage));
				if(n < 0)
				{
					perror("write");
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
		for (i = 0; i < size_pname; i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec); //conversion str octal -> int décimal


		/* Vérification + conversion (à gauche) que le fichier est à afficher */
		if(verif_convert_name(name, p.name, path_cwd) != 0)
		{
			/* Ici name a son nom exact */
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
				strcat(affichage, " ");

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
				strcat(affichage, " ");

				/* ajoute taille à message */
				sprintf(size_str, "%i", size_dec);
				strcat(affichage, size_str);
				strcat(affichage, "  ");

				/* récupère la date de dernière modification + ajout */
				for (i = 0; i < sizeof(p.mtime); i++)
				{
					p.mtime[i] = buf[136+i];
				}
				oct_to_dec(p.mtime, time_str);
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
				strcat(affichage, "  ");
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
