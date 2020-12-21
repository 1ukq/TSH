#include "utils_rdr.h"

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

int rdr_type(char * string, char * cwd){

	if(nb_rdr(string) > 1){
		//trop de redirections
		return -1;
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
			return -1;
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
			return -1;
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
			return -1;
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
			return -1;
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
			return -1;
		}
		else{
			token = strtok(NULL, " ");
		}
	}
	return 0;
}

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
			return file;
		}
		token = strdup(next_token);
	}
	return NULL;
}

/*
int main(void){
	char cwd[] = "/home/rf/Bureau/U2021/Projets/tsh/src/utils/";
	char input[] = "ls -kjl | > head 2>>> ../../dir/yo/hey";
	int n = rdr_type(input, cwd);
	printf("%i\n", n);
	if(n > 0){
		char * file = strdup(rdr_file(input, cwd));
		printf("%s\n", file);
	}

	return 0;
}
*/
