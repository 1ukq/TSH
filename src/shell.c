#include "shell.h"

int read_input(char ** tableau)
{
	int n;
	int indice = 0;




	char input[1024];

	n = write(STDOUT_FILENO, ">>> ", 4);
	if(n < 0)
	{
		perror("write");
		return -1;
	}

	n = read(STDOUT_FILENO, input, 1024);
	if(n < 0)
	{
		perror("read");
		return -1;
	}

	input[n-1] = '\0';




	//char * input = readline(">>> "); //add -lreadline to gcc (sudo apt-get install libreadline6 libreadline6-dev)
	char * input_copy = strdup(input);
	//free(input);
	char * token = strtok(input_copy, " ");

	while(token != NULL)
	{
		tableau[indice] = strdup(token);
		indice++;
		token = strtok(NULL, " ");
	}

	return indice;
}

int shell(void)
{
	struct sigaction action;

	int n, i;
	_Bool run = 1;
	_Bool execute = 1;

	char cd_beg_error[] = "cd: ";
	char cd_end_error[] = ": No such directory\n";

	char command_not_found[] = ": command not found\n";

	char * tableau[100];

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char introduction[] = "\n\n\n\n\t\tTSHELL - by Alessio, Lucas & Luka\n\n\n\n";



	/* introduction */
	n = write(STDOUT_FILENO, introduction, strlen(introduction));
	if(n < 0)
	{
		perror("write introduction");
		return -1;
	}




	/* bloquer les signaux */
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = SIG_IGN;

	for(i = 1; i <= 64; i++)
	{
		sigaction(i, &action, NULL);
	}




	/* running loop */
	while(run)
	{
		execute = 1;

		n = read_input(tableau);
		if(n < 0)
		{
			perror("read_input");
			return -1;
		}

		if(n > 0)
		{
			tableau[n] = NULL;

			/* exit ? */
			if(strcmp(tableau[0], "exit") == 0)
			{
				//exit shell
				//raise(SIGKILL);
				return 0;
			}

			/* cd ? */
			if((strcmp(tableau[0], "cd") == 0))
			{
				n = chdir(tableau[1]);
				getcwd(cwd, PATH_MAX);

				if(n < 0)
				{
					if(tableau[1] != NULL)
					{
						char error[sizeof(cd_beg_error) + sizeof(cd_end_error) + sizeof(tableau[1])];
						sprintf(error, "%s", cd_beg_error);
						strcat(error, tableau[1]);
						strcat(error, cd_end_error);

						n = write(STDERR_FILENO, error, strlen(error));
						if(n < 0)
						{
							perror("write cd error");
							return -1;
						}
					}
				}
			}
			else
			{
				/* unique commande avec ou sans options n'impliquant pas de tar */
				switch(fork())
				{
					case -1 :
						perror("fork");
						return -1;

					case 0:
						//fils
						execvp(tableau[0], tableau);
						/* gestion des erreurs */
						if(errno = ENOENT)
						{
							char error[sizeof(command_not_found) + sizeof(tableau[0])];
							sprintf(error, "%s" , tableau[0]);
							strcat(error, command_not_found);

							n = write(STDERR_FILENO, error, strlen(error));
							if(n < 0)
							{
								perror("write error");
								return -1;
							}
						}
						return -1;

					default :
						//père
						wait(NULL);
				}
			}
		}
	}
	return 0;
}




int main(void)
{
	shell();

	return 0;
}
