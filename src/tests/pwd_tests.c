#include "../pwd.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


#define PATH_MAX 4096

int pwd_test(void){
	int n, return_value;
	char buf[PATH_MAX];
	char cwd[PATH_MAX];

	getcwd(cwd, PATH_MAX);
	strcat(cwd, "/targets_bis");

	char expected_output[strlen(cwd) + 1];
	sprintf(expected_output, "%s\n", cwd);

	int fd = open("targets_bis/test_dir_pwd/test_pwd.txt", O_WRONLY | O_CREAT, 0666);
	if(fd < 0){
		perror("open");
		exit(1);
	}

	n = dup2(fd, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	return_value = pwd(cwd);

	close(fd);

	n = dup2(0, STDOUT_FILENO);
	if(n < 0){
		perror("dup2");
		exit(1);
	}

	fd = open("targets_bis/test_dir_pwd/test_pwd.txt", O_RDONLY);
	if(fd < 0){
		perror("open");
		exit(1);
	}

	n = read(fd, buf, PATH_MAX);
	if(n < 0){
		perror("read");
		exit(1);
	}
	close(fd);

	if(return_value == 0 && strcmp(buf, expected_output) == 0){
		printf("++++++test for pwd passed++++++\n");
	}
	else{
		printf("------test for pwd failed------\n");
	}

	return 0;
}

int main(void){

	system("bash script_bis_rm.sh");
	system("bash script_bis.sh");

	pwd_test();

	system("bash script_bis_rm.sh");

  return 0;
}
