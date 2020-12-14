#include "../list_file.h"

int main(int argc, char ** argv){
	int i, n, ret = 0;
	int ind_option = 0;
	char option[PATH_MAX];

	if(argc >= 2){
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				ind_option = i;
				sprintf(option,"%s",argv[i]);
				break;
			}
		}

		for(i = 1; i < argc; i++){
			if(i != ind_option){
				// on lance ls
				n = ls(((ind_option == 0)? NULL : option), argv[i]);

				if(n == -1){
					// erreur de type lecture/écriture dans ls
					return -1;
				}
				else if(n == -2){
					// erreur -> chemin invalide
					char error[] = "ls: cannot access: No such file or directory\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main : write");
						return -1;
					}
				}
				else if(n == -3){
					// erreur -> option invalide
					char error[] = "ls: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("ls_main : write");
						return -1;
					}
				}
				else{
					ret += n;
				}
			}
		}
	}
	else{
		// pas assez d'arguments donné au main
		ret = -2;
	}

	return ret;
}
