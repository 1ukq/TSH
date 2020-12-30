#include "../copy.h"
#include "../utils/fonctions_utiles.h"
#include <sys/types.h>
#include <sys/wait.h>
/*
int main(int argc, char ** argv){
	int n, i, option = 0, dest_tar, ret = 0, fd;

	if(argc > 2){

		// recherche de l'option si il y en a une
		for(i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				option = 1;
				if(strcmp(argv[i],"-r") != 0){
					// option invalide
					char error[] = "cp: invalid option\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("cp_main write1");
						return -1;
					}
					return 0;
				}
			}
		}

		// get path_to_tar_dest & path_in_tar_dest
		struct work_directory wd_dest;
		char path_to_tar_dest[sizeof(wd_dest.c_htar) + 1 + sizeof(wd_dest.tar_name)];
		char path_in_tar_dest[sizeof(wd_dest.c_tar)];

		fill_wd(argv[argc-1], &wd_dest);
		sprintf(path_to_tar_dest, "%s/%s", wd_dest.c_htar, wd_dest.tar_name);
		sprintf(path_in_tar_dest, "%s", wd_dest.c_tar);

		dest_tar = (strlen(wd_dest.tar_name) == 0 ? 0 : 1);

		// boucle d'éxecution
		for(i = 1; i < argc-1; i++){
			if(argv[i][0] != '-'){
				// c'est bien un chemin et pas une option

				// get path_to_tar_source & path_in_tar_source
				struct work_directory wd_source;
				char path_to_tar_source[sizeof(wd_source.c_htar) + 1 + sizeof(wd_source.tar_name)];
				char path_in_tar_source[sizeof(wd_source.c_tar)];

				fill_wd(argv[i], &wd_source);
				sprintf(path_to_tar_source, "%s/%s", wd_source.c_htar, wd_source.tar_name);
				sprintf(path_in_tar_source, "%s", wd_source.c_tar);

				//lancement des fonctions
				if(dest_tar){
					//la destination implique un tar
					if(strlen(wd_source.tar_name) != 0){
						//source:tar -> dest:tar
						if(option){
							//cp_tar_tar -r
							n = copy_from_tar_to_tar_r(path_to_tar_source, path_to_tar_dest, path_in_tar_source, path_in_tar_dest);
						}
						else{
							//cp_tar_tar
							//on enlève le / à la fin de path_in_tar_source
							path_in_tar_source[strlen(path_in_tar_source)-1] = '\0';

							//on enlève le / à la fin de path_in_tar_dest
							path_in_tar_dest[strlen(path_in_tar_dest)-1] = '\0';

							n = copy_from_tar_to_tar(path_to_tar_source, path_to_tar_dest, path_in_tar_source, path_in_tar_dest);
						}
					}
					else{
						//source:non-tar -> dest:tar
						if(option){
							//cp_nontar_tar -r
							n = copy_in_tar_r(argv[i], path_to_tar_dest, path_in_tar_dest);
						}
						else{
							//cp_nontar_tar
							//on enlève le / à la fin de path_in_tar_dest
							path_in_tar_dest[strlen(path_in_tar_dest)-1] = '\0';

							n = copy_in_tar(argv[i], path_to_tar_dest, path_in_tar_dest);
						}
					}
				}
				else{
					//la destination n'implique pas de tar
					if(strlen(wd_source.tar_name) != 0){
						//source:tar -> dest:non-tar
						if(option){
							//cp_tar_nontar -r
							n = copy_from_tar_r(path_to_tar_source, path_in_tar_source, argv[argc-1]);
						}
						else{
							//cp_tar_nontar
							fd = open(argv[argc-1], O_RDWR);
							if(fd < 0){
								n = -2;
							}
							else{
								//on enlève le / à la fin de path_in_tar_source
								path_in_tar_source[strlen(path_in_tar_source)-1] = '\0';

								n = copy_from_tar(path_to_tar_source, path_in_tar_source, fd);
							}
						}
					}
					else{
						//source:non-tar -> dest:non-tar (cas non traité)
						n = -2;
					}
				}

				if(n == -1){
					//erreur interne
				}
				else if(n == -2){
					//chemin invalide
					char error[] = "cp: No such file or directory\n";

					n = write(STDERR_FILENO, error, strlen(error));
					if(n < 0){
						perror("cp_main write");
						return -1;
					}
				}
				else{
					ret += n;
				}
			}
		}

	}
	else{
		//pas assez d'arguments
		char error[] = "cp: missing operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("rm_main write");
			return -1;
		}
	}


  return ret;
}
*/

int main(){
	build_tree_structure("test.tar", "dir", "./");
}
