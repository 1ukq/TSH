#include "tshell.h"
/*
à lire avant d'utiliser. le problème des chemins n'est pas résolu pour le lancement des executables des fonctions. (je fais reference ici à la partie du parser)
*/

int read_input(char * input){
	int n;

	/* write prompt */
	n = write(STDOUT_FILENO, ">>> ", 5);
	if(n < 0){
		perror("write prompt");
		return -1;
	}

	/* read user input */
	n = read(STDOUT_FILENO, input, PATH_MAX);
	if(n < 0){
		perror("read user input");
		return -1;
	}

	//enlever le '\n' de l'input
	input[n-1] = '\0';
	return n;
}


int tshell(void){
	int n, i;
	_Bool run = 1;
	char input[PATH_MAX];

	struct sigaction action;

	/* bloquage les signaux */
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = SIG_IGN;
	for(i = 1; i <= 64; i++)
	{
		sigaction(i, &action, NULL);
	}

	/* init cwd */
	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	/* get path_to_tsh */
	char path_to_tsh[PATH_MAX];
	getcwd(path_to_tsh, PATH_MAX);

	/* boucle principale */
	while(run){

		/* read user input */
		n = read_input(input);
		if(n < 0){
			return -1;
		}

		/* exit ? */
		if(strcmp(input, "exit") == 0){
			return 0;
		}

		if(n > 1){
			/* get redirection type and file */
			int redirection_type = rdr_type(input, cwd);
			char redirection_file[PATH_MAX];
			if(redirection_type > 0){
				sprintf(redirection_file, "%s", rdr_file(input, cwd));
			}
			else{
				sprintf(redirection_file, "%s", "");
			}

			/* artificially delete redirection from input */
			if(redirection_type == 1 || redirection_type == 6){
				input[strstr(input, " < ") - input] = '\0';
			}
			else if(redirection_type == 2 || redirection_type == 7){
				input[strstr(input, " > ") - input] = '\0';
			}
			else if(redirection_type == 3 || redirection_type == 8){
				input[strstr(input, " >> ") - input] = '\0';
			}
			else if(redirection_type == 4 || redirection_type == 9){
				input[strstr(input, " 2> ") - input] = '\0';
			}
			else if(redirection_type == 5 || redirection_type == 10){
				input[strstr(input, " 2>> ") - input] = '\0';
			}
			else if(redirection_type == -1){
				//unusual redirection
				char unusual_symbol_error[] = "bash: unusual use of symbol\n";
				n = write(STDERR_FILENO, unusual_symbol_error, strlen(unusual_symbol_error));
				if(n < 0){
					perror("write redirection error");
				}
				continue;
			}

			/* parser */
			char *** tab = parser(input, cwd, path_to_tsh);
			//printf("%d\n", redirection_type);

			//cmds_launcher(tab, redirection_type, redirection_file);

			//si tu as besoin de voir le contenu du tableau rétabli le code ci-dessous
			printf("%s\n", redirection_file);
			
			i = 0;
			int j;
			printf("[ ");
			while(tab[i] != NULL){
				j = 0;
				printf("[ ");
				while(tab[i][j] != NULL){
					printf("%s ", tab[i][j]);
					j++;
				}
				printf("] ");
				i++;
			}
			printf("]");
			printf("\n");		
			
		}
	}
	return 0;
}

//main

int main(void){

	int n = tshell();

	return n;
}

