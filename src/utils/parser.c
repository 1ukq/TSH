#include "parser.h"

int count_cmd(char * string){
	int i = 0, nb_cmd = 0;

	while(string[i] != '\0'){
		if(string[i] == '|'){
			nb_cmd++;
		}
		i++;
	}

	return nb_cmd + 1;
}

int count_words(char * string){
	int i = 0, nb_words = 0;

	while(string[i] != '\0'){
		if(string[i] == ' '){
			nb_words++;
		}
		i++;
	}

	return nb_words;
}


char *** parser(char * input){
	int i = 0, j = 0;
	// compter le nombre de commandes à stocker
	int nb_cmd = count_cmd(input);

	// création du tableau à renvoyer
	char *** tab = malloc(sizeof(char **) * nb_cmd);

	// split input into an array of cmd
	char * tab_cmd[nb_cmd];

	char * input_copy = strdup(input);
	char * token = strtok(input_copy, "|");
	i = 0;
	while(token != NULL){
		tab_cmd[i] = strdup(token);
		i++;
		token = strtok(NULL, "|");
	}
	free(input_copy);

	// split cmd into words and put them in tab structure
	for(i = 0; i < nb_cmd; i++){

		j = 0;
		tab[i] = malloc(sizeof(char *) * count_words(tab_cmd[i]) + 1);

		char * cmd_copy = strdup(tab_cmd[i]);
		token = strtok(cmd_copy, " ");

		while(token != NULL){
			//rendre les chemins absolu
			tab[i][j] = strdup(token);
			j++;
			token = strtok(NULL, " ");
		}
		tab[i][j] = NULL;
		free(cmd_copy);
	}
	tab[i] = NULL;

	return tab;
}
//free tab

int main(void){
	char *** tab = parser("ls -l yo | head | mkdir prout");
	int i = 0, j;

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

	//free(tab);

	return 0;
}
