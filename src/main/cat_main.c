#include "../copy.h"
#include "../utils/fonctions_utiles.h"
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char ** argv){
	int n, i;

	if(argc > 1){
		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				// cat ne prends pas d'options dans lorsqu'il est appelé dans un tar
				char error[] = "cat: invalid option\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("cat_main write1");
					return -1;
				}
				return 0;
			}
		}

		for(i = 1; i < argc; i++){
			// implique un tar ?
			struct work_directory wd;
			fill_wd(argv[i], &wd);

			if(strlen(wd.tar_name) == 0){
				// pas de tar impliqué -> chemin invalide
				char error[] = "cat: No such file\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("cat_main write2");
					return -1;
				}
			}
			else{
				// le chemin implique un tar
				char path_to_tar[strlen(wd.c_htar) + 1 + strlen(wd.tar_name)];
				char path_in_tar[strlen(wd.c_tar)];

				sprintf(path_to_tar, "%s/%s", wd.c_htar, wd.tar_name);
				sprintf(path_in_tar, "%s", wd.c_tar);

				//on enlève le / à la fin de path_in_tar si il y est
				int taille = strlen(path_in_tar);
				if(taille > 0 && path_in_tar[taille-1] == '/'){
					path_in_tar[strlen(path_in_tar)-1] = '\0';
				}

				// applique le cat
				n = cat(path_to_tar, path_in_tar);
				if(n == -2){
					// invalid path
					char error[] = "cat: No such file\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("cat_main write3");
						return -1;
					}
				}
			}
		}
	}
	else{
		// execute real cat
		n = fork();
		if(n < 0){
			perror("cat_main fork");
			return -1;
		}
		else if(n != 0){
			execlp("cat", "cat", NULL);
		}
		else{
			wait(NULL);
		}
	}

	return 0;
}
