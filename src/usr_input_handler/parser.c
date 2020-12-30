#include "parser.h"

/* Cette fonction permet de compter le nombre de commandes dans une chaine de caractères. On considère que chaque commande est séparée d'un pipe '|'. Lorsqu'on fait appelle à la fonction, c'est que l'utilisateur a tapé quelque chose, donc nb_cmd = 1. La fonction renvoie le nombre de commandes.*/
int count_cmd(char * string){
	int i = 0, nb_cmd = 1;

	while(string[i] != '\0'){
		if(string[i] == '|'){
			nb_cmd++;
		}
		i++;
	}

	return nb_cmd;
}

/* Cette fonction compte le nombre exacte de mots dans une chaine de caractères en se basant sur le fait que chaque mot est séparé d'au moins un ' '. */
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

/* Cette fonction est utilisée directement dans le shell. À partir de l'input de l'utilisateur brut (toutefois sans '\n' et sans redirections), elle renvoie un tableau composé des commandes à lancer et des arguments de celles-ci. C'est aussi ici qu'est déterminé si il faut appeler une commande impliquant un tar ou non. Grâce au tableau, il suffit d'appliquer exec aux differentes sous-listes pour lancer les bonnes executions.
Pour une entrée du type input = "ls -l Bureau/test.tar | head", la fonction renvoie
[ ["/home/tsh/src/execs/ls_t" , "-l" , chemin_absolu(Bureau/test.tar)],  [head] ]*/
char *** parser(char * input, char * cwd, char * path_to_tsh){
	int i = 0, j = 0;
	// compter le nombre de commandes à stocker
	int nb_cmd = count_cmd(input);

	// création du tableau à renvoyer
	char *** tab = malloc(sizeof(char **) * nb_cmd +1);

	// split input en tableau de commandes
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

	// split chaque commande en mots à insérer dans la structure de tab
	for(i = 0; i < nb_cmd; i++){

		char * cmd_copy = strdup(tab_cmd[i]);
		token = strtok(cmd_copy, " ");

		/* comme pwd_t ne prends pas d'arguments aux yeux de l'utilisateur, on lui ajoute le cwd artificiellement et surpprime les arguments en trop si il y en a */
		if(strcmp(token, "pwd") == 0 && strstr(cwd, ".tar") != NULL){
			tab[i] = malloc(sizeof(char *) * 3);
			tab[i][0] = strdup(token);
			tab[i][1] = strdup(cwd);
			tab[i][2] = NULL;
		}
		/* Même chose pour ls_t et ls_t -l, mais il faut faire attention à garder toutes les options */
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
		/* Cas général pour la transformation des commandes en mots et leur ajout dans tab*/
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

	// création de la structure pour derminer si la commande implique un tar
	int implique_tar[nb_cmd];
	for(i = 0 ; i < nb_cmd; i++){
		implique_tar[i] = 0;
	}

	i = 0;
	while(tab[i] != NULL){
		j = 1;
		while(tab[i][j] != NULL){
			if(implique_tar[i] == 0 && tab[i][j][0] != '-'){
				// tab[i][j] n'est donc ni une option, ni une commande directe, on cherche son chemin absolu
				char * path_abs = strdup(chemin_absolu(cwd, tab[i][j]));

				// cette partie est utile pour la transition d'un tar vers un non-tar avec les fonctions usuelles uniquement
				if(strstr(tab[i][j], "..") != NULL && strstr(cwd, ".tar") != NULL){
					if(tab[i][j][strlen(tab[i][j])-1] != '/'){
						path_abs[strlen(path_abs)-1] = '\0';
					}
					tab[i][j] = strdup(path_abs);
				}

				// si il y a un ".tar" dans le mot, on considère qu'il implique un tar
				if(strstr(path_abs, ".tar") != NULL){
					implique_tar[i] = 1;
				}
				free(path_abs);
			}
			j++;
		}
		i++;
	}

	/* Dans cette partie, rendons les chemin absolu pour les fonctions impliquant des tar et donnons le chemin vers les executables de ces fonctions*/
	i = 0;
	while(tab[i] != NULL){
		if(implique_tar[i]){
			char name[strlen(tab[i][0])];
			sprintf(name, "%s", tab[i][0]);

			if(strcmp(name, "cat") == 0){
				char path_to_cat[PATH_MAX];
				sprintf(path_to_cat, "%s/%s", path_to_tsh, "src/execs/cat_t");

				tab[i][0] = strdup(path_to_cat);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "cp") == 0){
				char path_to_cp[PATH_MAX];
				sprintf(path_to_cp, "%s/%s", path_to_tsh, "src/execs/cp_t");

				tab[i][0] = strdup(path_to_cp);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "mv") == 0){
				char path_to_mv[PATH_MAX];
				sprintf(path_to_mv, "%s/%s", path_to_tsh, "src/execs/mv_t");

				tab[i][0] = strdup(path_to_mv);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "ls") == 0){
				char path_to_ls[PATH_MAX];
				sprintf(path_to_ls, "%s/%s", path_to_tsh, "src/execs/ls_t");

				tab[i][0] = strdup(path_to_ls);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "pwd") == 0){
				char path_to_pwd[PATH_MAX];
				sprintf(path_to_pwd, "%s/%s", path_to_tsh, "src/execs/pwd_t");

				tab[i][0] = strdup(path_to_pwd);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "rm") == 0){
				char path_to_rm[PATH_MAX];
				sprintf(path_to_rm, "%s/%s", path_to_tsh, "src/execs/rm_t");

				tab[i][0] = strdup(path_to_rm);;
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "rmdir") == 0){
				char path_to_rmdir[PATH_MAX];
				sprintf(path_to_rmdir, "%s/%s", path_to_tsh, "src/execs/rmdir_t");

				tab[i][0] = strdup(path_to_rmdir);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "mkdir") == 0){
				char path_to_mkdir[PATH_MAX];
				sprintf(path_to_mkdir, "%s/%s", path_to_tsh, "src/execs/mkdir_t");

				tab[i][0] = strdup(path_to_mkdir);
				j = 1;
				while(tab[i][j] != NULL){
					if(tab[i][j][0] != '-'){
						tab[i][j] = strdup(chemin_absolu(cwd, tab[i][j]));
					}
					j++;
				}
			}
			else if(strcmp(name, "cd") == 0){
				//cd n'a pas d'executable car il modifie directement une variable "globale" à savoir cwd définie dans tshell
			}
			else{
				//fonction non reconnue
			}
		}
		i++;
	}

	return tab;
}
