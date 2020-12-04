#include "../cd.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define PATH_MAX 4096

void cd_tar_tar(void){
	int n;

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char nwd[PATH_MAX];
	sprintf(nwd, "%s/%s", cwd, "targets/test.tar/targets");

	//on met le cwd dans le tar
	strcat(cwd, "/targets/test.tar/targets/test_dir");

	//on applique cd
	n = cd(cwd, nwd);

	//vérification du résultat
	if(strcmp(cwd, nwd) == 0 && n == 0){
		printf("++++++test for cd tar -> tar passed++++++\n");
	}
	else{
		printf("------test for cd tar -> tar failed------\n");
	}
}

void cd_htar_tar(void){
	int n;

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char nwd[PATH_MAX];
	sprintf(nwd, "%s/%s", cwd, "targets/test.tar/targets");

	//le cwd est bien hors-tar (dans .../tests)

	//on applique cd
	n = cd(cwd, nwd);

	//vérification du résultat
	if(strcmp(cwd, nwd) == 0 && n == 0){
		printf("++++++test for cd hors-tar -> tar passed++++++\n");
	}
	else{
		printf("------test for cd hors-tar -> tar failed------\n");
	}
}

void cd_tar_htar(void){
	int n;

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char nwd[PATH_MAX];
	sprintf(nwd, "%s", cwd);

	//on met le cwd dans le tar
	strcat(cwd, "/targets/test.tar/targets/test_dir");

	//on applique cd
	n = cd(cwd, nwd);

	//vérification du résultat
	if(strcmp(cwd, nwd) == 0 && n == 0){
		printf("++++++test for cd tar -> hors-tar passed++++++\n");
	}
	else{
		printf("------test for cd tar -> hors-tar failed------\n");
	}
}

void cd_fail(void){
	int n;

	//on met le cwd dans le tar
	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	strcat(cwd, "/targets/test.tar/targets/test_dir");

	char cwd_expected[PATH_MAX];
	sprintf(cwd_expected, "%s", cwd);

	char nwd[PATH_MAX];
	sprintf(nwd, "%s/%s", cwd, "not_a_dir");

	//on applique cd
	n = cd(cwd, nwd);

	//vérification du résultat
	if(n == -2 && strcmp(cwd, cwd_expected) == 0){
		printf("++++++test for cd fail passed++++++\n");
	}
	else{
		printf("------test for cd fail failed------\n");
	}
}

int main(void){

	int n;
	char real_cwd[PATH_MAX];
	getcwd(real_cwd, PATH_MAX);

	/* tar -> tar */
	system("./script_rm.sh");
	system("./script.sh");

	cd_tar_tar();

	/* hors-tar -> tar */
	n = chdir(real_cwd);
	if(n < 0){
		perror("chdir 1");
		return -1;
	}

	system("./script_rm.sh");
	system("./script.sh");

	cd_htar_tar();

	/* tar -> hors-tar */
	n = chdir(real_cwd);
	if(n < 0){
		perror("chdir 2");
		return -1;
	}

	system("./script_rm.sh");
	system("./script.sh");

	cd_tar_htar();

	/* fail */
	n = chdir(real_cwd);
	if(n < 0){
		perror("chdir 3");
		return -1;
	}

	system("./script_rm.sh");
	system("./script.sh");

	cd_fail();

  return 0;
}
