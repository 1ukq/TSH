#include "cd.h"

int cd(struct work_directory * ad_cwd, struct work_directory * ad_nwd)
{
	struct posix_header p;

	char buf[BLOCK_SIZE];

	int shift, n, i, size_dec;



	/* vérfie que le nouveau chemin hors-tar existe et change "vraiment" de directory */
	if(chdir((*ad_nwd).c_htar) < 0)
	{
		/* le nouveau chemin hors-tar n'existe pas; on ne change pas de cwd */
		return 0;
	}

	/* vérifie que le nouveau chemin implique un tar */
	if(strlen((*ad_nwd).tar_name) <= 0)
	{
		/* le nouveau chemin n'implique pas de tar; on est sûr que nwd existe; cwd devient nwd */
		*ad_cwd = *ad_nwd;

		return 0;
	}

	/* vérifie que le tar existe à l'emplacement indiqué */
	int fd = open((*ad_nwd).tar_name, O_RDONLY);
	if(fd < 0)
	{
		/* le tar n'existe pas à l'endroit indiqué; le nwd n'existe donc pas; on reste dans l'ancien cwd -> on se remet dans le répertoire d'origine */
		chdir((*ad_cwd).c_htar);

		return 0;
	}

	/* vérifie que le nouveau chemin dans le tar est non-vide (sinon on est sûr que le chemin existe) */
	if(strlen((*ad_nwd).c_tar) <= 0)
	{
		/* le nouveau chemin dans le tar est vide; on sûr que nwd existe; cwd devient nwd */
		*ad_cwd = *ad_nwd;

		return 0;
	}

	/* lit en boucle les noms des repertoires dans le tar et vérifie qu'il en existe un de nom nwd.c_tar (= vérfie que le nouveau chemin dans le tar existe) */
	while(read(fd, buf, BLOCK_SIZE ) > 0)
	{
		if(buf[0] == '\0')
		{
			/* on arrive à la fin du tar => le nouveau chemin dans le tar n'existe pas; on se remet dans le répertoire d'origine; cwd reste inchangé */
			close(fd);

			chdir((*ad_cwd).c_htar);

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


			if(strcmp((*ad_nwd).c_tar, p.name) == 0)
			{
				/* le nouveau chemin dans le tar existe => nwd existe; cwd devient nwd */
				*ad_cwd = *ad_nwd;

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
