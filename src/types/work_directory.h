#ifndef WORK_DIRECTORY
#define WORK_DIRECTORY

#define PATH_MAX 4096

struct work_directory
{
  	char c_htar[PATH_MAX/2 - 100];	/* chemin hors-tar "/home/.../dossier" */
	char tar_name[200]; 			/* nom du tar "tar_name" */
  	char c_tar[PATH_MAX/2 - 100];	/* chemin dans le tar de nom tar_name "dos1/dos2/.../dosk/" */
};


#endif
