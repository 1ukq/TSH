#include "../removeDir.c"
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
	int i, n;

  if(argc >= 2){
		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				// rmdir ne prends pas d'options dans lorsqu'il est appelé dans un tar
				char error[] = "rmdir: invalid option\n";
				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("rmdir_main write1");
					return -1;
				}
				return 0;
			}
		}

		for(i = 1; i < argc; i++){
			// applique rmdir basique
			if(strstr(argv[i], ".tar") == NULL){
				// execute real rmdir
				n = fork();
				if(n < 0){
					perror("rmdir_main fork");
					return -1;
				}
				else if(n != 0){
					execlp("rmdir", "rmdir", argv[i], NULL);
				}
				else{
					wait(NULL);
				}
			}
			else{
        n = removedir(argv[i]);
			}

			if(n == -1){
				// erreur du type read/write -> voir makedir.c
				//n'est jamais censé arriver
				char error[] = "rmdir: error in removeDir.c code\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("rmdir_main write2");
					return -1;
				}
			}
			else if(n == -2){
				// invalid path
				char error[] = "rmdir: cannot remove directory: No such directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("rmdir_main write4");
					return -1;
				}

			}
			else if(n == -3){
				// directory is not empty
				char error[] = "rmdir: cannot remove directory: Directory not empty\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("rmdir_main write3");
					return -1;
				}
			}
		}
	}
	else{
		// mauvais nombre d'arguments
		char error[] = "rmdir: missing operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("rmdir_main write");
			return -1;
		}
	}
	return 0;
}
