#include "../list_file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define PATH_MAX 4096

void ls_test(void){

	int fd1, fd2, n, nb_file;
	char buf1[500], buf2[500];

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char path[PATH_MAX];
	sprintf(path, "%s/%s", cwd, "targets_bis/test.tar/test_dir/");

	fd1 = open("targets_bis/test_dir_ls/test_ls.txt", O_WRONLY | O_CREAT, 0666);
	if(fd1 < 0){
		perror("open");
		exit(1);
	}

	n = dup2(fd1, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	nb_file = ls(NULL, path);

	close(fd1);

	n = dup2(0, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	fd1 = open("targets_bis/test_dir_ls/test_ls.txt", O_RDONLY);
	if(fd1 < 0){
		perror("open");
		exit(1);
	}

	fd2 = open("targets_bis/test_dir_ls/ls_expected.txt", O_RDONLY);
	if(fd2 < 0){
		perror("open");
		exit(1);
	}

	n = read(fd1, buf1, 500);
	if(n < 0){
		perror("read");
		exit(1);
	}
	close(fd1);

	n = read(fd2, buf2, 500);
	if(n < 0){
		perror("read");
		exit(1);
	}
	close(fd2);

	if(strncmp(buf1, buf2, strlen(buf2)) == 0 && nb_file == 2){
		printf("++++++test for ls passed++++++\n");
	}
	else{
		printf("------test for ls failed------\n");
	}
}

void lsl_test(void){

	int fd1, fd2, n, nb_file;
	char buf1[500], buf2[500];

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char path[PATH_MAX];
	sprintf(path, "%s/%s", cwd, "targets_bis/test.tar/");

	fd1 = open("targets_bis/test_dir_ls/test_ls.txt", O_WRONLY | O_CREAT, 0666);
	if(fd1 < 0){
		perror("open");
		exit(1);
	}

	n = dup2(fd1, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	nb_file = ls("-l", path);

	close(fd1);

	n = dup2(0, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	fd1 = open("targets_bis/test_dir_ls/test_ls.txt", O_RDONLY);
	if(fd1 < 0){
		perror("open");
		exit(1);
	}

	fd2 = open("targets_bis/test_dir_ls/lsl_expected.txt", O_RDONLY);
	if(fd2 < 0){
		perror("open");
		exit(1);
	}

	n = read(fd1, buf1, 500);
	if(n < 0){
		perror("read");
		exit(1);
	}
	close(fd1);

	n = read(fd2, buf2, 500);
	if(n < 0){
		perror("read");
		exit(1);
	}
	close(fd2);

	if(strncmp(buf1, buf2, strlen(buf2)) == 0 && nb_file == 2){
		printf("++++++test for ls -l passed++++++\n");
	}
	else{
		printf("------test for ls -l failed------\n");
	}
}

void ls_fail(void){
	int nb_file;

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char path[PATH_MAX];
	sprintf(path, "%s/%s", cwd, "targets_bis/test.tar/test_dir_bis/not_a_dir");

	nb_file = ls("-l", path);

	if(nb_file == -2){
		printf("++++++test for ls fail passed++++++\n");
	}
	else{
		printf("------test for ls fail failed------\n");
	}
}


int main(void){

	/* ls */
	system("bash script_bis_rm.sh");
	system("bash script_bis.sh");

	ls_test();

	/* ls -l */
	system("bash script_bis_rm.sh");
	system("bash script_bis.sh");

	lsl_test();

	/* ls fail */
	system("bash script_bis_rm.sh");
	system("bash script_bis.sh");

	ls_fail();

	system("bash script_bis_rm.sh");
  return 0;
}
