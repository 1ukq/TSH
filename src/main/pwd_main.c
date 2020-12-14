#include "../pwd.h"

int main(int argc, char ** argv){
	int ret;

	if(argc == 2){
		// applique pwd
		ret = pwd(argv[1]);
	}
	else{
		// mauvais nombre d'arguments donné au main
		ret = -2;
	}

	return ret;
}
