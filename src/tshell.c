#include "tshell.h"

int read_input(char * input){
	int n;

	/* write prompt */
	n = write(STDOUT_FILENO, ">>> ", 4);
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

		printf("%s\n", input);
	}
}


int main(void){
	int n = tshell();

	printf("%i\n", n);

	return 0;
}
