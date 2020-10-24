***TSH - Premier rendu***
=========================

## Introduction
Ce fichier permet de faciliter la compréhension du projet dans son ensemble. Chaque fonction codée pour ce premier rendu y est décrite pour fournir au lecteur une approche complémentaire au code brut.

Nous nous sommes principalement penchés sur les fonctions directement liées au `tar` à savoir (jusqu'ici), `ls`, `ls -l`, `mkdir`, `rm`, `cp`, `cat`, `mv`. Ces fonctions faciliteront la poursuite du projet.

Une fois toutes les fonctions impliquant directement un `tar` codées, nous nous pencherons sur le shell en lui-même. L'utilisation de la fonction `exec()` paraît primordiale pour le comportement du shell hors tarball, et nous nous aiderons des fonctions ci-dessus lorsque les manipulations seront liées à un `tar`.

---

## `ls` et `ls -l`

```C
#include "list_file.c"

int ls(int fd_out, char option, const char * path_tar, const char * path_cwd);
```

##### Renvoie et arguments :
- `ls(...)` : renvoie le nombre de fichiers en path_cwd si tout s'est bien passé et `-1` sinon
- `fd_out` : sortie pour affichage (`STDOUT_FILENO` pour les tests) ce sera le *file descriptor* du *tshell* plus tard
- `option` : `' '` pour un `ls` et `'l'` pour un `ls -l`
- `path_tar` : chemin du `tar` (exemple `"chemin/.../test.tar"`)
- `path_cwd` : chemin de la localisation de l'utilisateur (`cwd`) à partir du `tar` avec un `'/'` à la fin (exemple `"dos1/dos2/dos3/"`) ou `""` si il est à la racine du `tar`


##### Description :
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


##### Remarques :
- Il est important de noter que `path_cwd` existe car seul la fonction cd sera en mesure de modifier le `path_cwd` dans le programme pour le **tsh**. Par ailleurs, lors d'un changement de `cwd`, `cd` vérifiera à chaque fois que le changement est possible.
- `ls -l` n'affiche pas (encore) le nombre de liens pour chaque fichier.
- Le sous-programme est structuré de la manière suivante : `list_file.c` contient que les fonctions utiles à `ls()` et `ls()`; `list_file.h` contient tous les `#include` et `#define`


##### Exemple de test :
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



# `mkdir`

```C
#include "mkdir.c"

int makedir(int fd_out, char * path_tar, char * path_cwd, char * dir_name);
```

##### Renvoie et arguments :
- `makedir(...)` : renvoie `0` si tout s'est bien passé et `-1` sinon
- `fd_out` : sortie pour affichage (`STDOUT_FILENO` pour les tests) ce sera le *file descriptor* du *tshell* plus tard
- `path_tar` : chemin du `tar` (exemple `"chemin/.../test.tar"`)
- `path_cwd` : chemin de la localisation de l'utilisateur (`cwd`) à partir du `tar` avec un `'/'` à la fin (exemple `"dos1/dos2/dos3/"`) ou `""` si il est à la racine du tar
- `dir_name` : nom du nouveau répertoire (avec ou sans `'/'` à la fin; ne doit pas contenir de `'/'` avant la fin)


##### Description :
- Crée la variable `fullname` = `path_cwd` + `dir_name` (+ `'/'` si il n'y est pas déjà).
- Ouvre le fichier `tar` en mode lecture/écriture puis lit l'en-tête du premier fichier du `tar` si il en contient, sinon voir point 6.
- Récupère le nom du fichier et le compare avec `fullname`. Si ils sont égaux, le répertoire existe déjà, la fonction se termine : ferme le *file descriptor* du `tar`, écrit un message sur `fd_out` (le *file descriptor* du (futur) shell) et renvoie `0`.
- (Sinon) Récupère la taille du fichier et passe à l'en-tête suivant.
- La fonction réitère le processus jusqu'à arriver à la fin du dernier fichier du `tar`.
- Remplit de la bonne manière (conversions judicieuses...) les cases importantes de la structure `posix_header`, les autres sont remplies de `'\0'`.
- Place le curseur de lecture/écriture juste en dessous du contenu du dernier fichier du `tar` et écrit l'en-tête du répertoire.
- Écrit deux blocs de `'\0'` puis ferme le *file descriptor* du `tar`.
- Renvoie `0` si tout s'est bien passé.


##### Remarques :
- Pour l'instant, `dir_name` ne peut pas contenir de `'/'` avant le dernier caractère.
- `fd_out` n'est utile que dans le cas oú le répertoire existe déjà dans le `tar`.


##### Exemple de test :
Après avoir créé un fichier `test.tar`
```C
#include "mkdir.c"

int main(void)
{
    int n = makedir(STDOUT_FILENO, "test.tar", "", "test_dir");
    printf("%i\n", n);

    return 0;
}
```

