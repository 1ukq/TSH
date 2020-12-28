#include "../list_file.h"
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
	int i, n;
	int option = 0;

	if(argc >= 2){

		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				option = 1;
				if(strcmp(argv[i],"-l") != 0){
					// option invalide
					char error[] = "ls: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write1");
						return -1;
					}
					return 0;
				}
			}
		}

		for(i = 1; i < argc; i++){
			if(argv[i][0] != '-'){
				// on lance ls
				if(strstr(argv[i], ".tar") == NULL){
					// execute real ls
					n = fork();
					if(n < 0){
						perror("ls_main fork");
						return -1;
					}
					else if(n != 0){
						execlp("ls", "ls", argv[i], option ? "-l" : NULL, NULL);
					}
					else{
						wait(NULL);
					}
				}
				else{
					n = ls( option ? "-l" : NULL, argv[i]);
				}

				// gérer les erreurs
				if(n == -1){
					// erreur de type lecture/écriture (ne devrait jamais arriver)
					char error[] = "ls: error in list_file.c code\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write2");
						return -1;
					}

					return -1;
				}
				else if(n == -2){
					// chemin invalide
					char error[] = "ls: cannot access: No such directory\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write3");
						return -1;
					}
				}
				else if(n == -3){
					// ne rentre jamais ici
					// option invalide (ce cas est déjà vérifié plus haut)
					char error[] = "ls: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main write4");
						return -1;
					}

					return -3;
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
	return 0;
}
