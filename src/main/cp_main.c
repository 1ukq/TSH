#include "../copy.h"
#include "../utils/fonctions_utiles.h"
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv){
	int n, i, option = 0, fd;

	if(argc < 2){
		//pas assez d'arguments
		char error[] = "cp: missing operand\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("cp_main write1");
			return -1;
		}

		return 0;
	}

	//recherche des options
	for(i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			option = 1;
			if(strcmp(argv[i], "-r") != 0){
				//option invalide
				char error[] = "cp: invalid option\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("write 2 in cp_main");
					return -1;
				}
				return 0;
			}
		}
	}

	int last_path = -1;
	//recherche de la cible
	for(i = 1; i < argc; i++){
		if(argv[i][0] != '-'){
			last_path = i;
		}
	}
	if(last_path == -1){
		//pas des chemin de destination
		char error[] = "cp: no destination file or directory\n";

		n = write(STDERR_FILENO, error, strlen(error));
		if(n < 0){
			perror("cp_main write2");
			return -1;
		}

		return 0;
	}

	//init path_to_tar_dest & path_in_tar_dest
	struct work_directory wd_dest;
	char path_to_tar_dest[sizeof(wd_dest.c_htar) + 1 + sizeof(wd_dest.tar_name)];
	char path_in_tar_dest[sizeof(wd_dest.c_tar)];

	fill_wd(argv[last_path], &wd_dest);
	sprintf(path_to_tar_dest, "%s/%s", wd_dest.c_htar, wd_dest.tar_name);
	sprintf(path_in_tar_dest, "%s", wd_dest.c_tar);

	int dest_tar = (strlen(wd_dest.tar_name) == 0 ? 0 : 1);

	//boucle d'execution
	for(i = 0; i < argc; i++){
		if(argv[i][0] != '-' && i != last_path){
			//argv[i] est un chemin source, on le met dans la structure work_directory pour récupérer le chemin vers le tar et le chemin dans le tar
			struct work_directory wd_source;
			char path_to_tar_source[sizeof(wd_source.c_htar) + 1 + sizeof(wd_source.tar_name)];
			char path_in_tar_source[sizeof(wd_source.c_tar)];

			fill_wd(argv[i], &wd_source);
			sprintf(path_to_tar_source, "%s/%s", wd_source.c_htar, wd_source.tar_name);
			sprintf(path_in_tar_source, "%s", wd_source.c_tar);

			if(dest_tar){
				//la destination est dans un tar
				if(strlen(wd_source.tar_name) == 0){
					//non-tar -> tar
					if(option){
						//-r (dir to dir)
						n = copy_from_dir_to_tar_r(path_to_tar_dest, argv[i], path_in_tar_dest);
					}
					else{
						//(file to file)
						if(strlen(wd_dest.c_tar) > 0 && path_in_tar_dest[strlen(path_in_tar_dest)-1] == '/'){
							path_in_tar_dest[strlen(path_in_tar_dest)-1] = '\0';
						}

						n = copy_in_tar(wd_source.c_htar, path_to_tar_dest, path_in_tar_dest);
					}
				}
				else{
					if(strlen(wd_source.c_tar) == 0){
						//tentative de copie d'un fichier .tar dans un .tar: erreur
						n = -3;
					}
					else{
						//tar -> tar
						if(option){
							//-r (dir to dir)
							n = copy_from_tar_to_tar_r(path_to_tar_source, path_in_tar_source, path_to_tar_dest, path_in_tar_dest);
						}
						else{
							//(file to file)
							if(strlen(wd_dest.c_tar) > 0 && path_in_tar_dest[strlen(path_in_tar_dest)-1] == '/'){
								path_in_tar_dest[strlen(path_in_tar_dest)-1] = '\0';
							}
							if(strlen(wd_source.c_tar) > 0 && path_in_tar_source[strlen(path_in_tar_source)-1] == '/'){
								path_in_tar_source[strlen(path_in_tar_source)-1] = '\0';
							}

							n = copy_from_tar_to_tar(path_to_tar_source, path_to_tar_dest, path_in_tar_source, path_in_tar_dest);
						}
					}
				}
			}
			else{
				//la destination est hors d'un tar
				if(strlen(wd_source.tar_name) == 0){
					//non-tar -> non-tar execute real cp with ot without option
					n = -5;
				}
				else{
					//tar -> non-tar
					if(option){
						//-r (dir to dir)
						if(strlen(wd_source.c_tar) == 0){
							//copy d'un fichier .tar vers un répertoire avec option
							n = -4;
						}
						else{
							n = copy_from_tar_to_dir_r(path_to_tar_source, path_in_tar_source, argv[last_path]);
						}
					}
					else{
						//(file to file)
						fd = open(wd_dest.c_htar, O_WRONLY);
						if(fd < 0){
							//No such file for destination
							n = -2;
						}
						else{
							if(strlen(wd_source.c_tar) == 0){
								//copy d'un fichier .tar vers un répertoire sans option
								n = -4;
							}
							else{
								if(path_in_tar_source[strlen(path_in_tar_source)-1] == '/'){
									path_in_tar_source[strlen(path_in_tar_source)-1] = '\0';
								}
								n = copy_from_tar(path_to_tar_source, path_in_tar_source, fd);
							}
						}
					}
				}
			}
			if(n == -1){
				//erreur interne ne devrait pas arriver
			}
			else if(n == -2){
				//mauvais chemin pour source ou dest
				char error[] = "cp: cannot access source or destination: No such file or directory\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("cp_main write3");
					return -1;
				}
			}
			else if(n == -3){
				//cp un fichier .tar dans un fichier .tar : erreur
				char error[] = "cp: cannot copy .tar file in .tar file\n";

				n = write(STDERR_FILENO, error, strlen(error));
				if(n < 0){
					perror("cp_main write4");
					return -1;
				}
			}
			else if(n == -4){
				//execute real cp with or without option (source = file.tar)
				n = fork();
				if(n == -1){
					perror("cp_main fork1");
					return -1;
				}
				else if(n == 0){
					execlp("cp", "cp", path_to_tar_source, argv[last_path], option ? "-r" : NULL, NULL);
					return 0;
				}
				else{
					wait(NULL);
				}
			}
			else if(n == -5){
				//execute real cp with or without option (file to file)
				n = fork();
				if(n == -1){
					perror("cp_main fork2");
					return -1;
				}
				else if(n == 0){
					execlp("cp", "cp", path_to_tar_source, path_to_tar_dest, option ? "-r" : NULL, NULL);
					return 0;
				}
				else{
					wait(NULL);
				}
			}
			else{
				//tout va bien
			}
		}
	}

	return 0;
}
