#ifndef WORK_DIRECTORY
#define WORK_DIRECTORY

struct work_directory
{
  	char c_htar[100];	/* chemin hors-tar "/home/.../dossier" */
	char tar_name[100]; /* nom du tar "tar_name" */
  	char c_tar[100];	/* chemin dans le tar de nom tar_name "dos1/dos2/.../dosk/" */
};


#endif
