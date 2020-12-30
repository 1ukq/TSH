***Système d'exploitation - projet TSHELL***
========================

# Compilation et utilisation

Vous trouverez à la racine du projet un makefile. Pour compiler l'ensemble du projet mettez vous simplement à sa racine et exécutez la commande `make all`. Un certain nombre d'exécutables seront générés dont plusieurs dans le répertoire `src/execs/`,et un un directement à la racine du projet nommé `tshell`. Pour utiliser notre shell, lancez la commande `./tshell`. Enfin, soulignons que le projet ne compilera pas sous MacOS (et peut sous certaines distribution de Linux). En revanche, vous trouverez à la racine du projet un dockerfile générant une image de la distribution Alpine sur laquelle le projet compile parfaitement.
Voici la manière la plus sûre de compiler et d'utiliser le projet.
* Aller à la racine du projet
* Lancer les commandes `docker build -t tsh .` puis `docker run -ti tsh`
* Faire `cd home/tsh`
* Compiler avec `make all`
* Utiliser le shell avec `./tshell`
  
## Information Dockerfile

Nous avons peu d'expérience concernant l'utilisation de docker, nous préférons donc préciser ceci: la version de `tar` utilisée dans l'image Alpine est tar busybox (la commande `tar --version` affiche `tar (busybox) 1.31.1`), et il nous paraît important de préciser que certaines fonctions ne marcheront pas si elles sont utilisées sur des archives crées avec `GNU tar`. En effet, les arichives dans notre projet doivent toujours finir avec deux blocs remplis de `\0`, ni plus; ni moins.


# Fonctionnalités

En excluant, toutes les fonctionnalité liées aux archives tar, notre shell est utilisable exactement comme un shell d'une distribution de Linux: il dispose des mêmes commandes et fonctionnalités. Des commandes telles que `cmd1 | ... | cmdn` fonctionnent et ont le comportement attendu. Il est également de faire les redirections `<`, `>`, `>>`, `2>`, `2>>`. En revanche, les redirections plus générales, de la forme `x>&y`, de sont pas implémentées, et une redirection ne peut que se faire à la fin d'une suite de commandes.

Voici les commandes liées aux archives tar implémentées et fonctionnelles (ces commandes fonctionnent sur des chemins impliquant des tar):
* `cd` et `exit`
* `pwd`
* `mkdir`, `rmdir` et `mv`
* `cp`
* `rm` et `rm -r`
* `ls` et `ls -l`
* `cat`
* `<`, `>`, `>>`, `2>` et `2>>` : les redirections de l'entrée et de la sortie standard, ainsi que de la sortie d'erreur fonctionnent même avec de chemins impliquant de archives tar mais cette redirection doit être précisée uniquement en dernier argument d'une suite de commandes
* `|` les tubes impliquant ou non des fichier inclus dans des tar
* évidemment, les chemins absolus ou relatifs sont utilisables

Nous pouvons cependant relever les problèmes suivants:
* Sur des chemins impliquant des tar `cp -r` n'a pas le comportement attendu
* également sur des chemins impliquant des tar, la commande `mv` dont l'argument `source` est un répertoire, pose différents problèmes suivant les cas
Nous reviendrons sur ces problèmes plus tard.
* `rm -r` ne supprime rien si un fichier dans un tar lui est donné en argument.
* Certains arguments rares dans l'utilisation traditionnelle d'un shell risquent de ne pas être pris en compte **dans des cas très spécifiques** impliquant un fichier .tar.
* `ls` ne fonctionne pas si on lui donne un fichier inclus dans un tar en argument.

# Difficultés et problèmes

## Droits d'utilisateur
Durant ce projet nous avons su prendre des décisions cruciales pour respecter les délais. C'est pourquoi n'avons malheureusement pas pu prendre en compte les droits des fichiers modifiés par nos fonctions.

## Difficultés liées au temps

Il nous paraît important de préciser que nous avons investi beaucoup de notre temps dans ce projet. Malgré cet investissement, nous n'avons pas pu finir d'implémenter plusieurs fonctionnalités. Nous aurions par ailleurs voulu pouvoir fournir des tests complets et rigoureux de toutes nos fonctions. Vous trouverez dans le répertoire `src/tests` des fichiers de tests et un makefile auxquels nous n'avons pas apporté de modifications depuis longtemps. Etant donné que nous avons modifié nos fonctions depuis certains fichiers de test ne compilent plus. Par ailleurs, nous n'avons pas eu le temps de régler les probmèmes cités plus haut.

## Problèmes de `cp -r` et `mv`

### `mv`

La fonction mv sur un répertoire dans un tar ne fonctionne pas.

### `cp -r` et `cp`

* Notre implémentation de `cp` ne vérifie pas tous les cas d'un `cp` normal. La cible de notre commande `cp` doit toujours être un autre fichier et ne peut pas être un répertoire.
* Notre implémentation de `cp -r` est également incomplète: nous n'avons pas eu le temps d'écrire la fonction permettant de copier un répertoire (se situant à l'extérieur d'une archive tar) dans un répertoire se situant dans une archive tar.
