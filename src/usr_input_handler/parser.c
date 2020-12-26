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
			while(string[i+1] == ' '){
				i++;
			}
		}
		i++;
	}

	if(string[0] == ' '){
		nb_words--;
	}

	if(string[i-1] != ' '){
		nb_words++;
	}

	return nb_words;
}


char *** parser(char * input, char * cwd, char * path_to_tsh){
	int i = 0, j = 0;
	// compter le nombre de commandes à stocker
	int nb_cmd = count_cmd(input);

	// création du tableau à renvoyer
	char *** tab = malloc(sizeof(char **) * nb_cmd +1);

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

		char * cmd_copy = strdup(tab_cmd[i]);
		token = strtok(cmd_copy, " ");

		if(strcmp(token, "pwd") == 0 && strstr(cwd, ".tar") != NULL){
			tab[i] = malloc(sizeof(char *) * 3);
			tab[i][0] = strdup(token);
			tab[i][1] = strdup(cwd);
			tab[i][2] = NULL;
		}
		else if(strcmp(token, "ls") == 0){
			int add_cwd = 1;

			tab[i] = malloc(sizeof(char *) * count_words(tab_cmd[i]) + 2);
			tab[i][0] = strdup(token);

			token = strtok(NULL, " ");
			j = 1;
			while(token != NULL){
				if(add_cwd && token[0] != '-'){
					add_cwd = 0;
				}
				tab[i][j] = strdup(token);
				j++;
				token = strtok(NULL, " ");
			}
			if(add_cwd){
				tab[i][j] = strdup(cwd);
			}
			else{
				tab[i][j] = NULL;
			}
			tab[i][j+1] = NULL;

		}
		else{
			tab[i] = malloc(sizeof(char *) * count_words(tab_cmd[i]) + 1);
			j = 0;
			while(token != NULL){
				tab[i][j] = strdup(token);
				j++;
				token = strtok(NULL, " ");
			}
			tab[i][j] = NULL;
		}

		free(cmd_copy);
	}
	tab[i] = NULL;

	// get abs_path for path in tab
	int implique_tar[nb_cmd];
	for(i = 0 ; i < nb_cmd; i++){
		implique_tar[i] = 0;
	}

	i = 0;
	while(tab[i] != NULL){
		j = 0;
		while(tab[i][j] != NULL){
			if(j > 0 && tab[i][j][0] != '-'){
				// alors tab[i][j] est un chemin
				char path[strlen(tab[i][j])];
				sprintf(path, "%s", tab[i][j]);

				char * path_abs = strdup(chemin_absolu(cwd, path));

				if(strstr(path_abs, ".tar") == NULL){
					tab[i][j] = strdup(path);
				}
				else{
					tab[i][j] = strdup(path_abs);
				}

				free(path_abs);

				if(implique_tar[i] == 0){
					if(strstr(tab[i][j], ".tar") != NULL){
						implique_tar[i] = 1;
					}
				}

			}
			j++;
		}
		i++;
	}

	// get abs_path to execs for functions that implies a tar
	i = 0;
	while(tab[i] != NULL){
		if(implique_tar[i]){
			char name[strlen(tab[i][0])];
			sprintf(name, "%s", tab[i][0]);

			if(strcmp(name, "cat") == 0){
				char path_to_cat[PATH_MAX];
				sprintf(path_to_cat, "%s/%s", path_to_tsh, "src/execs/cat_t");

				tab[i][0] = strdup(path_to_cat);
			}
			else if(strcmp(name, "cp") == 0){
				char path_to_cp[PATH_MAX];
				sprintf(path_to_cp, "%s/%s", path_to_tsh, "src/execs/cp_t");

				tab[i][0] = strdup(path_to_cp);
			}
			else if(strcmp(name, "mv") == 0){
				char path_to_mv[PATH_MAX];
				sprintf(path_to_mv, "%s/%s", path_to_tsh, "src/execs/mv_t");

				tab[i][0] = strdup(path_to_mv);
			}
			else if(strcmp(name, "ls") == 0){
				char path_to_ls[PATH_MAX];
				sprintf(path_to_ls, "%s/%s", path_to_tsh, "src/execs/ls_t");

				tab[i][0] = strdup(path_to_ls);
			}
			else if(strcmp(name, "pwd") == 0){
				char path_to_pwd[PATH_MAX];
				sprintf(path_to_pwd, "%s/%s", path_to_tsh, "src/execs/pwd_t");

				tab[i][0] = strdup(path_to_pwd);
			}
			else if(strcmp(name, "rm") == 0){
				char path_to_rm[PATH_MAX];
				sprintf(path_to_rm, "%s/%s", path_to_tsh, "src/execs/rm_t");

				tab[i][0] = strdup(path_to_rm);;
			}
			else if(strcmp(name, "rmdir") == 0){
				char path_to_rmdir[PATH_MAX];
				sprintf(path_to_rmdir, "%s/%s", path_to_tsh, "src/execs/rmdir_t");

				tab[i][0] = strdup(path_to_rmdir);
			}
			else if(strcmp(name, "mkdir") == 0){
				char path_to_mkdir[PATH_MAX];
				sprintf(path_to_mkdir, "%s/%s", path_to_tsh, "src/execs/mkdir_t");

				tab[i][0] = strdup(path_to_mkdir);
			}
			else if(strcmp(name, "cd") == 0){
				//cd n'a pas d'executable
			}
			else{
				//fonction non reconnue
			}
		}
		i++;
	}

	return tab;
}
//free tab

/*
int main(void){
	char *** tab = parser("ls -l ../../ok.tar/yoyoyoy/./../     ok/hey | mkdir prout", "/home/rf/Bureau/dir2/", "/home/rf/Bureau/U2021/Projets/tsh/");
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
*/
