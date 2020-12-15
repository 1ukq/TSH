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
		// mauvais nombre d'arguments donné au main (si erreur voir shell)
		n = -2;
	}

	return n;
}
