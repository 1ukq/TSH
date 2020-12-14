#include "move.c"

int main(int argc, char ** argv){
	int n, i, ret = 0;

	if(argc >= 3){
		for(i = 1; i < argc - 1; i++){
			n = mv(argv[i], argv[argc-1]);
			printf("%i\n", n);
			if(n == -1){
				//erreur de base (read write...)
				return -1;
			}
			if(n == -2){
				//path_file_source n'existe pas
				char error[] = "mv: cannot access source: No such file or directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mv_main write1");
					return -1;
				}
			}
			else if(n == -3){
				//path_file_dest n'existe pas
				char error[] = "mv: target is not a directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mv_main write2");
					return -1;
				}
			}
			else{
				ret += n;
			}
		}
	}
	else{
		// pas assez d'arguments donnés au main
		ret = -2;
	}

	return ret;
}
