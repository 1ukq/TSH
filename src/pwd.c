#include "pwd.h"

/* pwd agit qu'en simple "afficheur" de ce qu'on lui passe en argument, on lui donnera à chaque fois la variable "globale" cwd. La fonction renvoie 0 si tout s'est bien passé, -1 sinon. */
int pwd(char * path_cwd)
{
	int n;
	char message[strlen(path_cwd) + 1];

	sprintf(message, "%s", path_cwd);
	strcat(message, "\n");

	n = write(STDOUT_FILENO, message, strlen(message));
	if(n < 0)
	{
		perror("write");
		return -1;
	}

	return 0;
}
