#include "tshell.h"

/*Cette fonction permet de récupérer l'entrée de l'utilisateur et d'afficher le prompt*/
int read_input(char * input){
	int n;

	/* write prompt */
	n = write(STDOUT_FILENO, "### ", 4);
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

/* Cette fonction se lance au début du tshell, elle vide la fenêtre et affiche un titre */
int introduction(void){
	int n;
	char intro[] = "\n\t\t\t~ TSHELL by Alessio, Lucas & Luka ~\n\n";

	n = fork();
	switch (n) {
		case -1:
			perror("fork in introduction");
			return -1;
		case 0:
			execlp("clear", "clear", NULL);
			return -1;
		default:
			wait(NULL);
	}
	n = write(STDOUT_FILENO, intro, strlen(intro));
	if(n < 0){
		perror("write intro");
		return -1;
	}
	return 0;
}

/* tshell */
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
	char path_to_tsh[] = "/home/tsh";
	//les lignes ci-dessous fonctionnent uniquement si le shell est lancé à la racine du projet
	//char path_to_tsh[PATH_MAX];
	//getcwd(path_to_tsh, PATH_MAX);

	/* introduction */
	n = introduction();

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
			if(redirection_type == STDIN || redirection_type == STDIN_TAR){
				input[strstr(input, " < ") - input] = '\0';
			}
			else if(redirection_type == STDOUT_1 || redirection_type == STDOUT_1_TAR){
				input[strstr(input, " > ") - input] = '\0';
			}
			else if(redirection_type == STDOUT_2 || redirection_type == STDOUT_2_TAR){
				input[strstr(input, " >> ") - input] = '\0';
			}
			else if(redirection_type == STDERR_1 || redirection_type == STDERR_1_TAR){
				input[strstr(input, " 2> ") - input] = '\0';
			}
			else if(redirection_type == STDERR_2 || redirection_type == STDERR_2_TAR){
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

			/* cd ? */
			if((strcmp(tab[0][0], "cd") == 0)){
				// applique cd
				if(tab[0][1] != NULL){
					n = cd(cwd, chemin_absolu(cwd, tab[0][1]));
				}
				else{
					n = cd(cwd, "/");
				}
				// gère erreur de cd
				if(n == -2){
					char error[] = "bash: cd: No such directory\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("write cd error");
						return -1;
					}
				}
				// do redirection here for cd ?
			}
			else{
				/* Commands launcher */
				cmds_launcher(tab, redirection_type, redirection_file);
				wait(NULL);
			}
		}
	}
	return 0;
}

/* main principal pour le shell */
int main(void){

	int n = tshell();

	return n;
}
