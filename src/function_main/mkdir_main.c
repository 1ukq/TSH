#include "../makedir.h"

int main(int argc, char ** argv){
	int i, n, ret = 0;

	if(argc >= 2){
		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				// mkdir ne prends pas d'options dans lorsqu'il est appelé dans un tar
				char error[] = "mkdir: invalid option\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main write1");
					return -1;
				}
				return 0;
			}
		}

		for(i = 1; i < argc; i++){
			// applqiue makedir
			n = makedir(argv[i]);

			if(n == -1){
				// erreur du type read/write -> voir makedir.c
				//n'est jamais censé arriver
				char error[] = "mkdir: error in makedir.c code\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main write2");
					return -1;
				}

				ret -= 1;
			}
			else if(n == -2){
				// invalid path
				char error[] = "mkdir: cannot create directory: No such file or directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main write3");
					return -1;
				}

			}
			else if(n == -3){
				// file already exists
				char error[] = "mkdir: cannot create directory: File exists\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mkdir_main write4");
					return -1;
				}
			}
			else{
				ret += n;
			}
		}
	}
	else{
		// mauvais nombre d'arguments
		char error[] = "mkdir: missing operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("mkdir_main write");
			return -1;
		}
	}
	return ret;
}
