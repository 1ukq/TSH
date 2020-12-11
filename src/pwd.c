#include "pwd.h"

int pwd(char * path_cwd)
{
	int n;
	char message[strlen(path_cwd) + 1];

	// ajoute un '\n' au cwd à afficher
	sprintf(message, "%s\n", path_cwd);

	// affiche le cwd dans STDOUT
	n = write(STDOUT_FILENO, message, strlen(message));
	if(n < 0)
	{
		perror("write");
		return -1;
	}

	return 0;
}

int main(int argc, char ** argv)
{
	int n;

	if(argc == 2){
		n = pwd(argv[1]);
	}
	else{
		n = -2; // mauvais nombre d'arguments donné au main
	}

	return n;
}
