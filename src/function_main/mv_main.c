#include "../move.h"

int main(int argc, char ** argv){
	int n, i, ret = 0;

	if(argc >= 3){
		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				// mv ne prends pas d'options dans lorsqu'il est appelé dans un tar
				char error[] = "mv: invalid option\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mv_main write1");
					return -1;
				}
				return 0;
			}
		}

		//boucle principale, on considère que mv f1 f2 f3 f4 d (=) mv f1 d ; mv f2 d ; ...
		for(i = 1; i < argc-1; i++){
			// on applique mv
			n = mv(argv[i], argv[argc-1]);

			if(n == -1){
				//gérer les erreurs
				ret -= 1;
			}
			else if(n == -2){
				//path_file_source ou path_file_dest n'existe pas
				char error[] = "mv: cannot access source or destination: No such file or directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("mv_main write2");
					return -1;
				}
				ret -= 1;
			}
			else{
				ret += n;
			}
		}
	}
	else{
		//pas assez d'arguments
		char error[] = "mv: missing file operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("mv_main write");
			return -1;
		}

		return -2;
	}

	return ret;
}
