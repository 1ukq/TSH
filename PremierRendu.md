***TSH - Premier rendu***
=========================

# Introduction
Ce fichier permet de faciliter la compréhension du projet dans son ensemble. Chaque fonction codée pour ce premier rendu y est décrite pour fournir au lecteur une approche complémentaire au code brut.

Nous nous sommes principalement penchés sur les fonctions directement liées au `tar`. Ces fonctions faciliteront la poursuite du projet.

Une fois toutes les fonctions impliquant directement un `tar` codées, nous nous pencherons sur le shell en lui-même. L'utilisation de la fonction `exec()` paraît primordiale pour le comportement du shell hors tarball, et nous nous aiderons des fonctions ci-dessus lorsque les manipulations seront liées à un `tar`.



# `list_file.c` et `makedir.c`

## `ls` et `ls -l`

```C
#include "list_file.c"

int ls(int fd_out, char option, const char * path_tar, const char * path_cwd);
```

### Retour et arguments :
- `ls(...)` : renvoie le nombre de fichiers en path_cwd si tout s'est bien passé et `-1` sinon
- `fd_out` : sortie pour affichage (`STDOUT_FILENO` pour les tests) ce sera le *file descriptor* du *tshell* plus tard
- `option` : `' '` pour un `ls` et `'l'` pour un `ls -l`
- `path_tar` : chemin du `tar` (exemple `"chemin/.../test.tar"`)
- `path_cwd` : chemin de la localisation de l'utilisateur (`cwd`) à partir du `tar` avec un `'/'` à la fin (exemple `"dos1/dos2/dos3/"`) ou `""` si il est à la racine du `tar`


### Description et comportement:
La fonction `ls()` avec l'option `' '` liste les fichiers présent à un endroit donné dans un tar donné. Avec l'option `'l'`, la liste affichée est plus détaillée.
- Ouvre le fichier `tar` en mode lecture puis lit l'en-tête du premier fichier du `tar` si il en contient, se termine : ferme le *file descriptor* du `tar` et renvoie `0`.
- Vérifie que le fichier est à afficher en regardant d'abord si `path_cwd` est en préfixe du nom complet du fichier puis si il n'est pas plus "profond" que la profondeur de `path_tar` + 1.
- Récupère le nom complet et la taille du fichier. Puis convertit le nom complet (de la forme `".../dos/nom_fichier"` ou `".../dos/nom_fichier/..."` par exemple) en nom exact du fichier (`"nom_fichier"`).
- si l'option est `' '`:
    - Écrit `"nom_fichier\t"` dans le *file descriptor* `fd_out` du (futur) shell.
- sinon l'option est `'l'`:
    - Récupère, convertit et ajoute à l'affichage le type du fichier (`p.typeflag`)
    - Récupère, convertit et ajoute à l'affichage les droits du fichier (`p.mode`)
    - Récupère et ajoute à l'affichage le *username* du fichier (`p.uname`)
    - Récupère et ajoute à l'affichage le *groupname* du fichier (`p.gname`)
    - Convertit en octets et ajoute à l'affichage la taille du fichier (`p.size`)
    - Récupère, convertit et ajoute à l'affichage la date de dernière modification du fichier (`p.mtime`)
    - Ajoute à l'affichage le nom exact du fichier (`p.name`)
    - Écrit l'affichage dans le *file descriptor* fd_out du (futur) shell
- Enfin grâce à la taille récupérée, la fonction passe directement à l'en-tête suivant et réitère le processus jusqu'à arriver à un en-tête dont le premier caractère est `'\0'` on sait alors qu'on arrive à la fin du fichier `tar`, on le ferme.
- La fonction renvoie le nombre de fichiers qu'elle a affichés.


### Remarques :
- Il est important de noter que `path_cwd` existe car seul la fonction cd sera en mesure de modifier le `path_cwd` dans le programme pour le **tsh**. Par ailleurs, lors d'un changement de `cwd`, `cd` vérifiera à chaque fois que le changement est possible.
- `ls -l` n'affiche pas (encore) le nombre de liens pour chaque fichier.
- Le sous-programme est structuré de la manière suivante : `list_file.c` contient que les fonctions utiles à `ls()` et `ls()`; `list_file.h` contient tous les `#include` et `#define`


### Exemple de test :
Après avoir créé un fichier `test.tar`

```C
#include "list_file.c"

int main(void)
{
    int n = ls(STDOUT_FILENO, 'l', "test.tar", "");
    printf("%i\n", n);

    return 0;
}
```



## `makedir`

```C
#include "makedir.c"

int makedir(int fd_out, char * path_tar, char * path_cwd, char * dir_name);
```

### Retour et arguments :
- `makedir(...)` : renvoie `0` si tout s'est bien passé et `-1` sinon
- `fd_out` : sortie pour affichage (`STDOUT_FILENO` pour les tests) ce sera le *file descriptor* du *tshell* plus tard
- `path_tar` : chemin du `tar` (exemple `"chemin/.../test.tar"`)
- `path_cwd` : chemin de la localisation de l'utilisateur (`cwd`) à partir du `tar` avec un `'/'` à la fin (exemple `"dos1/dos2/dos3/"`) ou `""` si il est à la racine du tar
- `dir_name` : nom du nouveau répertoire (avec ou sans `'/'` à la fin; ne doit pas contenir de `'/'` avant la fin)


### Description et comportement :
La fonction `makedir()` crée un répertoire vide de nom donné à un endroit donné dans un tar donné.
- Crée la variable `fullname` = `path_cwd` + `dir_name` (+ `'/'` si il n'y est pas déjà).
- Ouvre le fichier `tar` en mode lecture/écriture puis lit l'en-tête du premier fichier du `tar` si il en contient, sinon voir point 6.
- Récupère le nom du fichier et le compare avec `fullname`. Si ils sont égaux, le répertoire existe déjà, la fonction se termine : ferme le *file descriptor* du `tar`, écrit un message sur `fd_out` (le *file descriptor* du (futur) shell) et renvoie `0`.
- (Sinon) Récupère la taille du fichier et passe à l'en-tête suivant.
- La fonction réitère le processus jusqu'à arriver à la fin du dernier fichier du `tar`.
- Remplit de la bonne manière (conversions judicieuses...) les cases importantes de la structure `posix_header`, les autres sont remplies de `'\0'`.
- Place le curseur de lecture/écriture juste en dessous du contenu du dernier fichier du `tar` et écrit l'en-tête du répertoire.
- Écrit deux blocs de `'\0'` puis ferme le *file descriptor* du `tar`.
- Renvoie `0` si tout s'est bien passé.


### Remarques :
- Pour l'instant, `dir_name` ne peut pas contenir de `'/'` avant le dernier caractère.
- `fd_out` n'est utile que dans le cas oú le répertoire existe déjà dans le `tar`.


### Exemple de test :
Après avoir créé un fichier `test.tar`
```C
#include "makedir.c"

int main(void)
{
    int n = makedir(STDOUT_FILENO, "test.tar", "", "test_dir");
    printf("%i\n", n);

    return 0;
}
```

# `copy.c`

## `copy_from_tar` :

```C
#include "copy.c"

int copy_from_tar(const char *path_tar, const char *path_file_source, int fd_dest);
```

### Description :

La fonction `copy_from_tar` copie le contenu d'un fichier se situant dans un archive, dans un autre fichier.

### Retour et arguments :

- `path_tar` est le chemin (relatif ou absolu) de l'archive depuis laquelle on copie le fichier `path_file_source`.
- `path_file_source` correspond au chemin au sein de l'archive dans laquelle se situe le fichier dont on veut copier le contenu.
- `fd_dest` est le descripteur du fichier dans lequel on veut copier le contenu du fichier pointé par `path_file_source`.
- La fonction renvoie le nombre de caractères copiés si tout s'est bien passé, et `-1` sinon.

## `cat`

```C
#include "copy.c"

void cat(const char *path_tar, const char *path_file_source);
```

### Description :

`cat` permet d'afficher sur la sortie standard (à la manière de la commande cat traditionnelle) le contenu d'un fichier se situant dans un tarball.

### Retour et arguments :

- `path_tar` est le chemin (absolu ou relatif) du tarball contenant le fichier dont on veut afficher le contenu.
- `path_file_source` est le chemin au sein du tarball du fichier dont on veut affichier le contenu.
- La fonction ne renvoie rien.

## `create_file_header`

```C
#include copy.c

int create_file_header(const char *path_file_source, struct stat *restrict buf, struct posix_header *header);
```

### Description :

Cette fonction crée un entête valide d'un fichier afin d'insérer son contenu dans un tarball.

### Retour et arguments :

- `path_file_source` est le chemin du fichier à partir duquel on crée l'entête.
- `buf` est un pointeur vers une `struct stat` qui contient des informations relatives au fichier `path_file_source`.
- `header` est un pointeur vers une `struc posix_header`, que l'on va remplir afin de créer l'entête.
- Renvoie `0` si tout s'est bien passé, `-1` sinon.

## `insert_file_in_tar`

```C
#include copy.c

int insert_file_in_tar(const char *path_tar, const char *path_file_source);
```

### Description :

Cette fonction permet d'insérer le contenu de n'importe quel fichier ordinaire dans un tarball, avec l'entête du fichier correspondant, tout en maintenant l'intégrité du tarball.

### Retour et arguments :

- `path_tar` est le chemin du tarball dans lequel on copie le contenu du fichier.
- `path_file_source` est le chemin du fichier dont on copie le contenu.
- Renvoie `0` si tout s'est bien passé, `-1` sinon.

## D'autres fonctions :

Le fichier copy.c contient d'autres fonctions, qui sont des méthodes intérmédiaires utilisées uniquement pour les fonctions `insert_file_in_tar` et `create_file_header`.
Vous trouverez notamment des fonctions dont les noms ont pour préfixe `init_` qui permettent d'initialiser les champs d'une `struct posix_header` pour correctement créer un entête de fichier. Les fonctions `compare_buffers_of_same_size`, `copy_string`, `find_next_block` et `buffarize` permettent de correctement copier, au bon endroit, le contenu d'un fichier dans un tarball.

## Remarques sur le fichier copy.c :

Ce fichier n'est pas très bien organisé: il contient des fonctions qui seront utilisées pour implémenter différentes commandes, il conviendrait de scinder son contenu en plusiers fichiers afin de mieux factoriser le code.
De plus vous remarquerez que le nom de ce fichier n'est pas très pertinent quant au code qu'il contient.
Enfin il y a dans ce fichier quelques lignes de code en commentaire, qui sont en fait les lignes que nous utilisons lorsque nous voulons compiler sur MacOS. L'ensemble du code de ce fichier est adapté aux distributions Linux, et notamment à la distribution Alpine à partir de laquelle nous avons fait une image docker.

# `tests.c`

Vous trouverez dans ce fichier quelques tests unitaires de certaines fonctions implémentées. Ces tests unitaires sont exécutés avec le framework `munit`.

## Les fonctions tests :

- `copy_from_tar_test` vérifie, caractère par caractère, que le contenu du fichier du fichier copié est exactement ce qui a été copié. Elle vérifie également que la taille de ce qui a été copié correspond à la taille du fichier copié.
- `cat_test` appelle simplement la fonction cat, pour vérifier que celle-ci s'exécute correctement.
- `ls_test` appelle la fonction `ls` et vérifie que son retour correspond à nos attentes.
- Le fichier a un `main`, dans lequel on appelle les fonction `insert_file_in_tar` et `makedir` pour vérifier que celles-ci conservent l'intégrité des tarballs sur lesquels ont les appelle. Nous n'avons pas encore implémenter les tests unitaires de ces deux fonctions, nous vérifions à la main leur comportement.

# `Makefile`

Le projet a deux Makefiles. Un à la racine qui compile tout le projet et crée l'exécutable `tsh` (Cet exécutable est pour l'instant pratiquement vide), l'autre dans `src/tests` qui compile les tests unitaires et produit l'exécutable `tests`. Pour tester le projet, Il faut donc écrire dans le `main` de `tests.c`, compiler avec ce Makefile et exécuter `tests`.
Précisons que le Makefile à la racine dispose d'une cible `tests` (faire `make test`) pour compiler les tests.

# `Dockerfile`

Vous trouverez à la racine un Dockerfile, pour tester le projet.
