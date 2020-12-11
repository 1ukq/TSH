#include "makedir.h"

/* Cette fonction permet de récupérer le chemin jusqu'au sous-répertoire du dossier à créer, il nous permet de vérifier l'existence de l'emplacement pour créer le nouveau répertoire */
void get_prev_rep(char * path, char * path_prev_rep){

	int len = strlen(path);
	int i = len;
	int nb_slash = 0;

	sprintf(path_prev_rep, "%s", "");

	while(i >= 0 && nb_slash < 2){
		if(path[i] == '/'){
			nb_slash++;
		}
		i--;
	}

	if(i > 0){
		char * path_tempo = strndup(path, i+2);
		sprintf(path_prev_rep,"%s", path_tempo);
	}
}

/* Cette fonction permet d'initialiser l'entête du répertoire à insérer dans le tar. Elle renvoie 0 si tout s'est bien passé, -1 sinon */
int init_header_dir(char * path_in_tar, struct posix_header *p){

	// make header clean
	memset(p, '\0', BLOCK_SIZE);

	// fill header
	sprintf(p -> name, "%s", path_in_tar); //name
	sprintf(p -> mode, "%s", "0000755"); //mode
	sprintf(p -> uid, "%07o", getuid()); //uid
	sprintf(p -> gid, "%07o", getgid()); //gid
	sprintf(p -> size, "%011o", 0); //size
	sprintf(p -> mtime, "%lo", time(NULL)); //mtime
	p -> typeflag = DIRTYPE; //typeflag
	sprintf(p -> magic, "ustar"); //magic
	sprintf(p -> version, "00"); //version
	struct passwd * pswd = getpwuid(getgid()); //uname
	sprintf(p -> uname, "%s", (*pswd).pw_name); //--
	struct group * grp = getgrgid(getgid()); //gname
	sprintf(p -> gname, "%s", (*grp).gr_name);; //--
	set_checksum(p); //checksum

	// check checksum
	int n = check_checksum(p);
	if(!n){
		return -1;
	}

	return 0;
}

/* Ci-dessous la fonction principale du fichier. Elle prend en argument le chemin absolu du nouveau dossier (cad home/.../test.tar/.../sous_rep/nom_nouveau_rep).
Renvoie:
 	0 si le répertoire a bien été créé
	-1 si il y a eu une erreur de type read/write...
	-2 si le chemin est invalide (pas de tar ou le sous-répertoire n'existe pas...)
	-3 si le répertoire existe déjà */
int makedir(char * path)
{
	struct work_directory wd;
	char path_in_tar[sizeof(wd.c_tar)];
	char path_to_tar[sizeof(wd.c_htar) + 1 + sizeof(wd.tar_name)];

	struct posix_header p;
	memset(&p, '\0', BLOCK_SIZE);

	char buf[BLOCK_SIZE];

	char end_blocks[2*BLOCK_SIZE];
	memset(end_blocks, '\0', 2*BLOCK_SIZE);

	int i, n;
	int shift;
	int prev_rep_exist = 0;

	int size_psize = sizeof(p.size);
	int size_dec;
	int size_pname = sizeof(p.name);

	fill_wd(path, &wd); //remplie la structure définie dans types/work_directory.h
	if(strlen(wd.tar_name) == 0 || strlen(wd.c_tar) == 0)
	{
		// le chemin n'implique aucun tar -> chemin invalide
		return -2;
	}

	sprintf(path_to_tar, "%s/%s", wd.c_htar, wd.tar_name); //chemin extérieur au tar
	sprintf(path_in_tar, "%s", wd.c_tar); //chemin interne au tar

	char path_prev_rep[strlen(path_in_tar)];
	get_prev_rep(path_in_tar, path_prev_rep); //chemin du sous-répertoire dans le tar


	int fd = open(path_to_tar, O_RDWR);
	if(fd < 0){
		perror("open");
		return -2;
	}


	while(read(fd, buf, BLOCK_SIZE ) > 0)
	{
		if((buf[0] == '\0')){
			// on se place à la fin du tar

			if(!prev_rep_exist){
				// le sous-répertoire n'existe pas -> chemin invalide
				close(fd);
				return -2;
			}

			// initialisation de l'entête
			n = init_header_dir(path_in_tar, &p);
			if(n < 0){
				perror("init_header_dir");
				return -1;
			}

			// place le curseur en dessous du contenu du dernier fichier
			n = lseek(fd, (-1)*BLOCK_SIZE, SEEK_CUR);
			if(n < 0)
			{
				perror("lseek 2");
				return -1;
			}
			// écrit l'entête du nouveau répertoire
			n = write(fd, &p, BLOCK_SIZE);
			if(n < 0)
			{
				perror("write 1");
				return -1;
			}
			// écrit les 2 blocs de fin du tar
			n = write(fd, end_blocks, 2*BLOCK_SIZE);
			if(n < 0)
			{
				perror("write 2");
				return -1;
			}

			close(fd);

			return 0;
		}

		// récupère le nom du fichier
		for (i = 0; i < size_pname; i++){
			p.name[i] = buf[i];
		}

		// regarde si le répertoire n'est pas déjà existant
		if(strcmp(path_in_tar, p.name) == 0){
			// le répertoire existe deja
			close(fd);
			return -3;
		}

		// regarde si le sous-répertoire existe
		if(!prev_rep_exist){
			if(strlen(path_prev_rep) == 0 || strcmp(path_prev_rep, p.name) == 0){
				prev_rep_exist = 1;
			}
		}

		// récupère la taille du fichier + conversion pour passer à l'entête suivant
		for (i = 0; i < size_psize; i++)
		{
			p.size[i] = buf[124+i];
		}
		sscanf(p.size, "%o", &size_dec); //conversion str octal -> int décimal

		// passe à l'en-tête suivant
		shift = (size_dec + BLOCK_SIZE -1)/BLOCK_SIZE;
		n = lseek(fd, shift*BLOCK_SIZE, SEEK_CUR);
		if(n < 0)
		{
			perror("lseek 1");
			return -1;
		}
	}

	perror("read");
	return -1;
}
