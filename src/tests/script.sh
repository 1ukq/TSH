mkdir targets
mkdir targets/test_dir
touch targets/test_dir/foo
touch targets/test_dir/helloworld
touch targets/bar
touch targets/dest

echo > targets/bar "La chose, qui attendait, s'est alertée, elle a fondu sur moi, elle se coule en moi, j'en suis plein. - Ce n'est rien: la Chose,
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
Maintenant, je sens son poids au bout de mon bras. Elle tire un peu, à peine, mollement, moelleusement, elle existe.\c"

echo > targets/test_dir/foo "123456789
AZERTYUIO\c"

echo "Hello world !\c" > targets/test_dir/helloworld

tar -cf targets/test.tar targets


#ls & ls -l
mkdir targets/test_dir_ls
touch targets/test_dir_ls/ls_expected.txt
touch targets/test_dir_ls/lsl_expected.txt

echo "test_dir\tbar\tdest\t\n\c" > targets/test_dir_ls/ls_expected.txt

echo "drwxr-xr-x  rf/L  0  $(date +%F) $(date +%R)  test_dir\n-rw-r--r--  rf/L  19  $(date +%F) $(date +%R)  bar\n-rw-r--r--  rf/L  2090  $(date +%F) $(date +%R)  dest\n\c" > targets/test_dir_ls/lsl_expected.txt

#pwd
mkdir targets/test_dir_pwd
