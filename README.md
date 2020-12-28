***Système d'exploitation - projet TSHELL***

# Compilation et utilisation

Vous trouverez à la racine du projet un makefile. Pour compiler l'ensemble du projet mettez vous simplement à sa racine et exécutez la commande `make all`. Un certain nombre d'exécutables seront générés dont plusieurs dans le répertoire `src/execs/`,et un un directement à la racine du projet nommé `tshell`. Pour utiliser notre shell, lancez la commande `./tshell`. Il est important de préciser que notre shell doit être lancé depuis la racine du projet, et ne peut être lancer depuis n'importe quel répertoire sans rencontrer plusieurs problèmes durant son utilisation. Enfin, soulignons que le projet ne compilera pas sous MacOS (et peut sous certaines distribution de Linux). En revanche, vous trouverez à la racine du projet un dockerfile générant une image de la distribution Alpine sur laquelle le projet compile parfaitement.
Voici la manière la plus sûre de compiler et d'utiliser le projet.
* Aller à la racine du projet
* Lancer les commandes `docker build -t tsh .` puis `docker run -ti tsh`
* Faire `cd home/tsh`
* Compiler avec `make all`
* Utiliser le shell avec `./tshell`


# Fonctionnalités

En excluant, toutes les fonctionnalité liées aux archives tar, notre shell est utilisable exactement comme un shell d'une distribution de Linux: il dispose des mêmes commandes et fonctionnalités. Des commandes telles que `cmd1 | ... | cmdn` fonctionnent et ont le comportement attendu. Il est également de faire les redirections `<`, `>`, `>>`, `2>`, `2>>`. En revanche, les redirections plus générales, de la forme `x>&y`, de sont pas implémentées.

Voici les commandes liées aux archives tar implémentées et fonctionnelles (ces commandes fonctionnent sur des chemins impliquant des tar):
* `cd` et `exit`
* `pwd`
* `mkdir`, `rmdir` et `mv`
* `cp`
* `rm` et `rm -r`
* `ls` et `ls -l`
* `cat`
* les redirections de l'entrée et de la sortie standard, ainsi que de la sortie d'erreur fonctionnent même avec de chemins impliquant de archives tar
* évidemment, les chemins absolus ou relatifs sont utilisables

Nous pouvons cependant relever les problèmes suivants:
* Sur des chemins impliquant des tar `cp -r` n'a pas le comportement attendu
* également sur des chemins impliquant des tar, la commande `mv` dont l'argument `source` est un répertoire, pose différents probmèmes suivant les cas
Nous reviendrons sur ces problèmes plutard.

# Difficultés et problèmes

## Difficultés liées au temps

Il nous paraît important de dire que nous avons investi beaucoup de notre temps dans ce projet. Malgré cet investissement, nous n'avons pas eu le temps d'implémenter plusieurs fonctionnalités. Nous n'avons notamment pas eu le temps de faire des tests complets et rigoureux de nos fonctions. Vous trouverez dans le répertoire `src/tests` des fichiers de tests et un makefile auxquels nous n'avons pas apportés de modifications depuis longtemps. Cependant, comme nous avons modifié nos fonctions depuis il est probable que certains fichiers ne compilent plus. Par ailleurs, nous n'avons pas eu le temps de régler les probmèmes cités plus haut.

## Problèmes de `cp -r` et `mv`

### `mv`

### `cp -r`

### Droits utilisateur



