#!/bin/bash

#create tar
mkdir targets_bis
mkdir targets_bis/test_dir

touch targets_bis/test_dir/foo
touch targets_bis/test_dir/helloworld
touch targets_bis/bar

echo -n "La chose, qui attendait, s'est alertée, elle a fondu sur moi, elle se coule en moi, j'en suis plein. - Ce n'est rien: la Chose,
c'est moi. L'existence, libérée, dégagée, reflue sur moi. J'existe. J'existe. C'est doux, si doux, si lent. Et léger: on dirait
que ça tient en l'air tout seul. Ça remue. Ce sont des effleurements partout qui fondent et s'évanouissent. Tout doux, tout doux.
Il y a de l'eau mousseuse dans ma bouche. Je l'avale, elle glisse dans ma gorge, elle me caresse - et la voila qui renaît dans
ma bouche, j'ai dans la bouche à perpétuité une petite mare d'eau blanchâtre - discrète - qui frôle ma langue. Et cette mare,
c'est encore moi. Et la langue. Et la gorge, c'est moi. Je vois ma main, qui s'épanouit sur la table. Elle vit - c'est moi. Elle
s'ouvre, les doigts se déploient et pointent. Elle est sur le dos. Elle me montre son ventre gras. Elle a l'air d'une bête à la
renverse. Les doigts, ce sont les pattes. Je m'amuse à les faire remuer, très vite, comme les pattes d'un crabe qui est tombé sur
le dos. Le crabe est mort: les pattes se recroquevillent, se ramènent sur le ventre de ma main. Je vois les ongles - la seule chose
de moi qui ne vit pas. Et encore. Ma main se retourne, s'étale à plat ventre, elle m'offre à présent son dos. Un dos argenté, un peu
brillant - on dirait un poisson, s'il n'y avait pas les poils roux à la naissance des phalanges. Je sens ma main. C'est moi, ces deux
bêtes qui s'agitent au bout de mes bras. Ma main gratte une de ses pattes, avec l'ongle d'une autre patte; je sens son poids sur la
table qui n'est pas moi. C'est long, long, cette impression de poids, ça ne passe pas. Il n'y a pas de raison pour que ça passe.
A la longue, c'est intolérable... Je retire ma main, je la mets dans ma poche. Mais je sens tout de suite, à travers l'étoffe, la
chaleur de ma cuisse. Aussitôt, je fais sauter ma main de ma poche; je la laisse pendre contre le dossier de la chaise.
Maintenant, je sens son poids au bout de mon bras. Elle tire un peu, à peine, mollement, moelleusement, elle existe." > targets_bis/bar

echo -n "123456789
AZERTYUIO" > targets_bis/test_dir/foo

echo -n "Hello world !" > targets_bis/test_dir/helloworld

cd targets_bis

tar -cf test.tar test_dir bar

#ls & ls -l
mkdir test_dir_ls

touch test_dir_ls/ls_expected.txt
touch test_dir_ls/lsl_expected.txt

echo -n -e "helloworld\tfoo\t
" > test_dir_ls/ls_expected.txt

echo -n "drwxr-xr-x  $(id -u -n)/$(id -g -n)  0  $(date +%F) $(date +%R)  test_dir
-rw-r--r--  $(id -u -n)/$(id -g -n)  2090  $(date +%F) $(date +%R)  bar
" > test_dir_ls/lsl_expected.txt

#pwd
mkdir test_dir_pwd

#mkdir
mkdir test_dir_mkdir

touch test_dir_mkdir/res_expected
touch test_dir_mkdir/list_tar.sh

echo -n "drwxr-xr-x $(id -u -n)/$(id -g -n)         0 $(date +%T) $(date +%R) test_dir/
-rw-r--r-- $(id -u -n)/$(id -g -n)        13 $(date +%T) $(date +%R) test_dir/helloworld
-rw-r--r-- $(id -u -n)/$(id -g -n)        19 $(date +%T) $(date +%R) test_dir/foo
-rw-r--r-- $(id -u -n)/$(id -g -n)      2090 $(date +%T) $(date +%R) bar
drwxr-xr-x $(id -u -n)/$(id -g -n)         0 $(date +%T) $(date +%R) test_dir/makedir_test/
" > test_dir_mkdir/res_expected

echo -n "#!/bin/bash
tar -tvf targets_bis/test.tar
" > test_dir_mkdir/list_tar.sh
