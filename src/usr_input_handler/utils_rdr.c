#include "utils_rdr.h"

/* Cette fonction permet de compter le nombre de redirections dans une chaines de caractère. Pour cela on considère que seules les redirections peuvent contenir des des > ou < ainsi, même si celles si sont directement collés à un mot, elles seront prises en compte. */
int nb_rdr(char * string){
	int count = 0;
	char * string_copy = strdup(string);
	char * token = strtok(string_copy, " ");
	while(token != NULL){
		if(strchr(token, '>') != NULL || strchr(token, '<') != NULL){
			// si il y a > ou < alors il y a l'une des redirections suivantes : > >> 2> 2>> <
			count++;
		}
		token = strtok(NULL, " ");
	}

	return count;
}

/* Cette fonction permet de déterminer le type de redirection d'une chaine de caractères. Les types sont des entiers définis dans utils_rdr.h. */
int rdr_type(char * string, char * cwd){

	if(nb_rdr(string) > 1){
		//on ne considère au plus qu'une redirection, il y en a plusieurs, on s'arrête.
		return -2;
	}

	char * string_copy = strdup(string);
	char * token = strtok(string_copy, " ");
	while(token != NULL){
		// <
		if(strcmp(token, "<") == 0){
			token = strtok(NULL, " ");
			if(token != NULL){
				char * file = strdup(chemin_absolu(cwd, token));
				if(strstr(file, ".tar") != NULL){
					// < tar_file
					return STDIN_TAR;
				}
				// < file
				return STDIN;
			}
			// < None
			return -3;
		}
		// >
		else if(strcmp(token, ">") == 0){
			token = strtok(NULL, " ");
			if(token != NULL){
				char * file = strdup(chemin_absolu(cwd, token));
				if(strstr(file, ".tar") != NULL){
					// > tar_file
					return STDOUT_1_TAR;
				}
				// > file
				return STDOUT_1;
			}
			// > None
			return -3;
		}
		// >>
		else if(strcmp(token, ">>") == 0){
			token = strtok(NULL, " ");
			if(token != NULL){
				char * file = strdup(chemin_absolu(cwd, token));
				if(strstr(file, ".tar") != NULL){
					// >> tar_file
					return STDOUT_2_TAR;
				}
				// >> file
				return STDOUT_2;
			}
			// >> None
			return -3;
		}
		// 2>
		else if(strcmp(token, "2>") == 0){
			token = strtok(NULL, " ");
			if(token != NULL){
				char * file = strdup(chemin_absolu(cwd, token));
				if(strstr(file, ".tar") != NULL){
					// 2> tar_file
					return STDERR_1_TAR;
				}
				// 2> file
				return STDERR_1;
			}
			// 2> None
			return -3;
		}
		// 2>>
		else if(strcmp(token, "2>>") == 0){
			token = strtok(NULL, " ");
			if(token != NULL){
				char * file = strdup(chemin_absolu(cwd, token));
				if(strstr(file, ".tar") != NULL){
					// 2>> tar_file
					return STDERR_2_TAR;
				}
				// 2>> file
				return STDERR_2;
			}
			// 2>> None
			return -3;
		}
		else{
			token = strtok(NULL, " ");
		}
	}
	// aucunes redirections ou redirection non reconnue par la fonction
	return 0;
}

/* Cette fonction permet de déterminer quel fichier est lié à la redirection. Et renvoie son chemin absolu. */
char * rdr_file(char * string, char * cwd){
	//use rdr_file after verifying that rdr_type() > 0
	char * string_copy = strdup(string);
	char * token = strtok(string_copy, " ");
	//search for last word
	while(token != NULL){
		char * next_token = strtok(NULL, " ");
		//if next_token is NULL then the actual token is the last word
		if(next_token == NULL){
			//get absolute path of last word
			char * file = strdup(chemin_absolu(cwd, token));
			//on enlève le / de la fin
			if(file[strlen(file) - 1] == '/'){
				file[strlen(file)-1] = '\0';
			}

			return file;
		}
		token = strdup(next_token);
	}
	return NULL;
}
