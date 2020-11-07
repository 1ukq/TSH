#include "cd.h"

/* cette fonction agit comme un cd (dans un tar et aussi hors d'un tar) elle change la variable path_cwd entrée en argument par la variable path_nwd si celle-ci est correcte, et ne change pas sinon. De plus le changement "officiel" pour la partie du chemin hors-tar se fait avec un chdir. La fonction renvoie 0 si tout s'est passé comme prévu */
int cd(char * path_cwd, char * path_nwd)
{
	/* si el nouveau chemin est vide */
	if(strlen(path_nwd) == 0)
	{
		return 0;
	}

	/* rend le nwd chemin absolu */
	char path_nwd_complet[strlen(path_cwd) + 1 + strlen(path_nwd)];
	chemin_absolu(path_cwd, path_nwd, path_nwd_complet);

	struct posix_header p;

	char buf[BLOCK_SIZE];
	int shift, n, i, size_dec;

	struct work_directory cwd;
	fill_wd(path_cwd, &cwd);

	struct work_directory nwd;
	fill_wd(path_nwd_complet, &nwd);


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
