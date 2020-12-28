#include "../pwd.h"

int main(int argc, char ** argv){
	int n;

	if(argc == 2){
		n = pwd(argv[1]);
		if(n == -1){
			//erreur interne à pwd
			return -1;
		}
	}
	else{
		// mauvais nombre d'arguments donné au main (n'arrive jamais ici aux vues du parser)
		char error[] = "pwd: ignoring arguments\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("write pwd main");
			return -1;
		}

		n = -2;
	}

	return n;
}
