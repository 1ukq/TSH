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
	int n, i;
	_Bool run = 1;
	_Bool execute = 1;

	char * tableau[100];

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char introduction[] = "\t\tTSHELL - by Alessio, Lucas & Luka\n\n";

	n = write(STDOUT_FILENO, introduction, strlen(introduction));

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
				return 0;
			}

			/* cd ? */
			if((strcmp(tableau[0], "cd") == 0) && (n > 1))
			{
				n = chdir(tableau[1]);
			}

			/* unique commande avec ou sans options n'impliquant pas de tar */
			switch(fork())
			{
				case -1 :
					perror("fork");
					return -1;

				case 0:
					//fils
					execvp(tableau[0], tableau);

				default :
					//père
					wait(NULL);
			}
		}
	}

	//gérer les erreures (command not found...)



	return 0;
}

int main(void)
{
	shell();

	return 0;
}
