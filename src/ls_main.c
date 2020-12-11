#include "list_file.h"

int main(int argc, char ** argv){
	int i, n, ret = 0;
	int ind_option = argc; //argv[argc] = NULL

	if(argc >= 2){
		for(i = 1; i < argc; i++){
			if((argv[i][0] == '-')){
				ind_option = i;
				break;
			}
		}

		char option[] = argv[ind_option];

		for(i = 1; i < argc; i++){
			if(i != ind_option){
				// on lance ls
				n = ls(option, argv[i])
				if(n == -1){
					// erreur de type lecture/écriture dans ls
					return -1;
				}
				else if(n == -2){
					char error[] = "ls: cannot access: No such file or directory\n";

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
