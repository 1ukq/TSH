#include "parser.h"

int count_cmd(char * string){
	int i = 0;
	char * string_copy = strdup(string);
	char * token = strtok(string_copy, "|");

	while(token != NULL){
		i++;
		token = strtok(NULL, "|");
	}
	free(string_copy);
	return i;
}

int count_words(char * string){
	int i = 0;
	char * string_copy = strdup(string);
	char * token = strtok(string_copy, " ");

	while(token != NULL){
		i++;
		token = strtok(NULL, " ");
	}
	free(string_copy);
	return i;
}


char *** parser(char * input){
	int i = 0, j = 0;
	// compter le nombre de commandes à stocker
	int nb_cmd = count_cmd(input);
	int nb_words;

	// création du tableau à renvoyer
	char *** tab = malloc(sizeof(char **) * nb_cmd);

	// split cmd&args
	char * input_copy = strdup(input);
	char * split = strtok(input_copy, "|");
	j = 0;

	while(split != NULL){
		// regader si split implique tar si oui rendre le chemin absolu
		nb_words = count_words(split);

		char * split_copy = strdup(split);
		char * token = strtok(split_copy, " ");

		tab[i] = malloc(sizeof(char *) * (nb_words+1));

		while(token != NULL){
			tab[i][j] = malloc(1);
			sprintf(tab[i][j], "%s", token);
			j++;
			printf("%s %i\n",token ,*token);
			token = strtok(NULL, " ");
		}

		printf("%s %i\n", split, *split);
		i++;
		split = strtok(NULL, "|");
		free(split_copy);
	}
	free(input_copy);

	return tab;
}
//free tab

int main(void){
	char *** tab = parser("ls -l yo | head | mkdir prout");
	int i = 0, j;

	printf("\n\n\n");
	while(tab[i] != NULL){
		j = 0;
		while(tab[i][j] != NULL){
			printf("%s %i %i\n", tab[i][j], i, j);
			j++;
		}
		i++;
	}

	free(tab);

	return 0;
}
