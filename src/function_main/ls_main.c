#include "../list_file.h"

int main(int argc, char ** argv){
	int i, n, ind_option = -1;
	int ret = 0;
	char option[50];

	if(argc >= 2){

		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				//sauvegarde l'indice de l'option et l'option elle-même
				ind_option = i;
				sprintf(option, "%s", argv[i]);
				break;
			}
		}

		for(i = 1; i < argc; i++){
			if(i != ind_option){
				// on lance ls
				n = ls(ind_option == -1 ? NULL : option, argv[i]);

				// gérer les erreurs
				if(n == -1){
					// erreur de type lecture/écriture (ne devrait jamais arriver)
					char error[] = "ls: error in list_file.c code\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write1");
						return -1;
					}

					return -1;
				}
				else if(n == -2){
					// chemin invalide
					char error[] = "ls: cannot access: No such file or directory\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write2");
						return -1;
					}
				}
				else if(n == -3){
					// option invalide
					char error[] = "ls: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write3");
						return -1;
					}

					return -3;
				}
				else{
					ret += n;
				}
			}
		}
	}
	else{
		// pas assez d'arguments donné au main (ne devrait jamais s'afficher -> voir shell)
		char error[] = "ls: error in shell.c code\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("ls_main write4");
			return -1;
		}
		return -2;
	}


	// return ?
	return ret;
}
