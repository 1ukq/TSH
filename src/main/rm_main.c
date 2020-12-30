#include "../remove.h"
#include "../utils/fonctions_utiles.h"
#include <sys/types.h>
#include <sys/wait.h>

/* Ce fichier permet de faire de notre rm un executable. C'est aussi ici qu'on gère les erreurs de la fonction aux vues de ce à quoi on s'attend et de ce que renvoie rm */
int main(int argc, char ** argv){
	int n, i, option = 0;

	if(argc > 1){

		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				option = 1;
				if(strcmp(argv[i],"-r") != 0){
					// option invalide
					char error[] = "rm: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("rm_main write1");
						return -1;
					}
					return 0;
				}
			}
		}

		// boucle d'éxecution
		for(i = 1; i < argc; i++){
			if(argv[i][0] != '-'){
				// c'est bien un chemin et pas une option
				// implique un tar ?
				struct work_directory wd;
				fill_wd(argv[i], &wd);

				if(strlen(wd.tar_name) == 0){
					// pas de tar impliqué -> chemin invalide
					// on applique le vrai rm
					if(argv[i][strlen(argv[i])-1] == '/'){
						argv[i][strlen(argv[i])-1] = '\0';
					}
					n = fork();
					if(n < 0){
						perror("rm_main fork");
						return -1;
					}
					else if(n != 0){
						execlp("rm", "rm", argv[i], NULL);
					}
					else{
						wait(NULL);
					}
				}
				else{
					// le chemin implique un tar
					char path_to_tar[strlen(wd.c_htar) + 1 + strlen(wd.tar_name)];
					sprintf(path_to_tar, "%s/%s", wd.c_htar, wd.tar_name);

					if(strlen(wd.c_tar) == 0){
						// execute real rm on tar
						n = fork();
						if(n < 0){
							perror("rm_main fork");
							return -1;
						}
						else if(n != 0){
							execlp("rm", "rm", path_to_tar, NULL);
						}
						else{
							wait(NULL);
						}
					}
					else{
						char path_in_tar[strlen(wd.c_tar)];
						sprintf(path_in_tar, "%s", wd.c_tar);

						if(option){
							//applique rm -r
							n = remove_file_from_tar_r(path_to_tar, path_in_tar);
						}
						else{
							//on enlève le / à la fin de path_in_tar si il y est
							int taille = strlen(path_in_tar);
							if(taille > 0 && path_in_tar[taille-1] == '/'){
								path_in_tar[strlen(path_in_tar)-1] = '\0';
							}

							//applique rm
							n = remove_file_from_tar(path_to_tar, path_in_tar);
						}

						if(n == -2){
							// invalid path
							char error[] = "rm: No such file\n";

							n = write(STDERR_FILENO, error, strlen(error));
							if(n < 0){
								perror("rm_main write3");
								return -1;
							}
						}

						if(n == -3){
							// invalid path 2
							char error[] = "rm: No such directory\n";

							n = write(STDERR_FILENO, error, strlen(error));
							if(n < 0){
								perror("rm_main write3");
								return -1;
							}
						}
					}
				}
			}
		}
	}
	else{
		//pas assez d'arguments
		char error[] = "rm: missing operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("rm_main write4");
			return -1;
		}
	}

	return 0;
}
