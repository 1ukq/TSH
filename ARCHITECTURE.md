# Introduction
Pour ce projet, nous avons considérer l'utilisateur au centre de notre objectif. C'est pour cela que l'on a essayé de garder un fonctionnement très proche des shell traditionnels. Cependant cette apparence simple cache de nombreuses lignes de code.

# Structure du tshell
Le tshell se découpe en deux parties principales. La première concerne l'ensemble des fonctions impliquant un `tar`. La seconde concerne le shell lui-même et son fonctionnement.

## Fonctionnalités liées au `tar`
Les fonctions concernées sont celles de la forme `fonction.c` visibles directement dans `tsh/src`. Prenons une fonction type de nom `function`, celle-ci se découpe en trois fichiers, `tsh/src/function.c`, `tsh/src/function.h` et `tsh/src/main/function_main.c` (à un synonyme prêt). Les deux premiers fichiers concernent le corps de la fonction, le dernier est sa surface: ce fichier permet d'en faire un exécutable qui se retrouvera dans `tsh/src/execs`.

## Fichiers nécessaire au bon fonctionnement du shell
Les fichiers concernés dans cette section sont ceux dans les répertoires suivant: `tsh/src/types`, `tsh/src/utils`, `tsh/src/usr_input_handler`.
- `tsh/src/types` regroupe les structures spécifiques au projet utilisées
- `tsh/src/utils` regroupe des fonctions annexes utiles à différents niveaux du projets, à savoir liées aux fonctions impliquant des tar ou directement au shell et ses sous-fonctions
- `tsh/src/usr_input_handler` contient les fonctions les plus *proches* du shell, c'est là qu'on trouve notamment le parser (`parser.c`) et le lanceur de commandes (`cmds_launcher.c`) mais aussi le tshell lui-même (`tshell.c`) chef d'orchestre de tout le projet.

## Liens entre les fichiers
Il existe de nombreux liens entre les fichiers. Dans cette section ne seront mentionnés ni les liens évidents, ni les liens avec les fichiers des répertoires `tsh/src/types` et `tsh/src/utils` qui se répartissent sur l'ensemble du projet.
Partons de `tshell.c` celui-ci est lié aux fichiers `utils_rdr.c`, `parser.c` et `cmds_launcher.c`. `cmds_launcher.c` est lié à `redirection.c` et aux exécutables de `tsh/src/execs`. Chacun des exécutables du répertoire cité précédemment est directement créé à partir des fichiers de `tsh/src/main` eux-mêmes issus des fichiers en `.c` issus du répertoire `tsh/src`.

## Structure brute et descriptions brèves
- `tsh/`
	- `ARCHITECTURE.md`
	-	`AUTHORS`
	- `Dockerfile`
	- `Makefile` *pour lancer le tshell*
	- `README.md` *à lire absolument*
	- `src/`
		- `cd.c` & `cd.h` *cd vérifie que le nouveau chemin existe et remplace la variable globale cwd par ce nouveau chemin, sinon il ne fait rien*
		- `copy.c` & `copy.h`
		- `list_file.c` & `list_file.h` *ls parcourt toutes les entêtes du tar et affiche les noms (et informations) des entêtes dont le nom contient le chemin donné en argument*
		- `makedir.c` & `makedir.h` *mkdir parcourt toutes les entêtes et vérifie qu'il n'existe pas déjà un répertoire de même nom puis ajout l'entête appropriée en fin de tar*
		- `move.c` & `move.h` *cat se trouve ici*
		- `pwd.c` & `pwd.h` *pwd ne fait qu'afficher ce qu'on lui donne en argument. En pratique on lui donne à chaque fois la variable globale cwd*
		- `remove.c` & `remove.h`
		- `removeDir.c` & `removeDir.h` *rmdir parcourt toutes les entêtes du tar et supprime le répertoire concerné ssi il est vide*
		- `execs/` *rempli d'exécutables après avoir make*
		- `main/`*Ces mains permettent principalement de faire plusieurs appels en même temps sur une fonction et de créer des exécutables pour chacune d'entre elles. Remarque: étant donné que cd modifie la variable globale cwd dans le shell, nous avons décidé de ne pas appeler cd par un exécutable*
			- `cat_main.c`
			- `cp_main.c`
			- `ls_main.c`
			- `mkdir_main.c`
			- `mv_main.c`
			- `pwd_main.c`
			- `rm_main.c`
			- `rmdir_main.c`
		- `tests/` *contient quelques tests pour certaines fonctions, mais ce dossier n'a pas été mis à jour récemment*
		- `types/`
			- `posix_header.h`
			- `work_directory.h` *la structure qui y est définie permet de découper un chemin en trois parties: chemin hors-tar, nom du tar et chemin dans le tar*
		- `usr_input_handler/`
			- `cmds_launcher.c` & `cmds_launcher.h` *les tubes, les redirections et les appels de fonctions sont gérés ici*
			- `parser.c` & `parser.h`*permet de "convertir" l'entrée de l'utilisateur en renvoyant un tableau approprié pour faire les bonnes exécutions*
			- `redirection.c` & `redirection.h`
			- `tshell.c` & `tshell.h` *fichier principal du projet*
			- `utils_rdr.c` & `utils_rdr.h` *ces fonctions permettent de déterminer le type et le fichier de redirection*
		- `utils/`
			- `find_file.c` & `find_file.h`
			- `fonctions_utiles.c` & `fonctions_utiles.h` *la fonction chemin_absolu définie dans ce fichier renvoie un chemin absolu commençant et se terminant par un /, les autres fonctions sont fortement liées à work_directory.h*
			- `init_header.c` & `init_header.h`
			- `utils_string.c` & `utils_string.h`

# Fonctionnement du tshell
Dans cette section nous allons suivre le parcours d'une entrée de l'utilisateur pour se faire une idée du fonctionnement du tshell.
- Lancement du shell.
- Le shell gère les signaux et initialise les *variables globales* dont `cwd`.
- La boucle principale attend une entrée de l'utilisateur. Prenons `input = "ls -l test.tar/dir | head > file"`.
-	Le shell stocke le type de redirection et le fichier de redirection pour le transmettre aux fonctions qui gèrent ces redirections. L'entrée de l'utilisateur devient `input = "ls -l test.tar/dir | head"`.
- `input` entre dans le parser qui détermine quelles fonctions sont à appliquer et renvoie un tableau contenant les chemin des executables des fonctions à appliquer ainsi que de leurs arguments en chemin absolu si il le faut. On a alors `tableau = [ [ chemin absolu vers l'exécutable de ls impliquant des tar, -l, chemin absolu vers test.tar/dir ], [head] ]`
- Le tableau, le type et le fichier de redirection sont fournis au lanceur de commandes. Celui-ci se charge d'appliquer ce qu'impose l'utilisateur: les tubes, les redirections et l'exécution des commandes.
- On retourne en haut de la boucle principale

# Réponse au sujet
Après ces quelques mois de travail, nous avons réussi à rendre un shell fonctionnel sur un docker (voir `README.md`) qui répond aux grandes lignes du sujet. Néanmoins nous sommes conscients que certaines choses seraient à améliorer comme la gestion des erreurs, la propreté et la cohérence du code ou encore le déroulement de certaines fonctions. Cependant il nous a fallu faire des choix cruciaux pour respecter les délais imposés.
