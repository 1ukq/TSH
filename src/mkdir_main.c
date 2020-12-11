#include "makedir.h"

int main(int argc, char ** argv){
	int i, n, ret = 0;

	if(argc >= 2){
		for(i = 1; i < argc; i++){
			// applique makedir
			n = makedir(argv[i]);

			if(n == -1){
				// erreur de type affichage dans makedir
				ret = -1;
			}
			else if(n == -2){
				// chemin invalide
				char error[] = "mkdir: cannot create directory: No such file or directory";

				// affiche le message d'erreur
				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main : write1");
					return -1;
				}
			}
			else if(n == -3){
				// le répertoire eciste déjà
				char error[] = "mkdir: cannot create directory: File exists";

				// affiche le message d'erreur
				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main : write2");
					return -1;
				}
			}
		}
	}
	else{
		// mauvais nombre d'arguments donné au main
		ret = -2;
	}

	return ret;
}
