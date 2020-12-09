#include "../makedir.h"

#define PATH_MAX 4096

void makedir_test(void){

	char buf1[500], buf2[500];
	int n;

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);

	char path_abs[PATH_MAX];
	sprintf(path_abs, "%s/%s", cwd, "targets_bis/test.tar/test_dir/makedir_test/");

	int ret = makedir(path_abs);


	// fd1 = expectation
	int fd1 = open("targets_bis/test_dir_mkdir/res_expected", O_RDONLY);
	if(fd1 < 0){
		perror("open fd1");
		exit(1);
	}
	n = read(fd1, buf1, 500);
	if(n < 0){
		perror("read fd1");
		exit(1);
	}
	close(fd1);


	// fd2 = res
	int fd2 = open("targets_bis/test_dir_mkdir/res", O_WRONLY | O_CREAT, 0666);
	if(fd2 < 0){
		perror("open fd2");
		exit(1);
	}

	n = dup2(fd2, STDOUT_FILENO);
	if(n < 0){
		perror("dup2 fd2");
		exit(1);
	}

	system("bash targets_bis/test_dir_mkdir/list_tar.sh"); // write res in res

	close(fd2);

	fd2 = open("targets_bis/test_dir_mkdir/res", O_RDONLY);
	if(fd2 < 0){
		perror("open fd2 (2)");
		exit(1);
	}

	n = read(fd2, buf2, 500); // put res in buf2
	if(n < 0){
		perror("read fd2");
		exit(1);
	}

	close(fd2);

	//reset settings for standard output
	n = dup2(0, STDOUT_FILENO);
	if(n < 0){
		perror("dup2 0");
		exit(1);
	}

	// compare buf1 and buf2 (=) compare res and expectation
	printf("%s\n", buf1);
	printf("%s\n", buf2);
	if(strcmp(buf1, buf2) == 0 && ret == 0){
		printf("++++++test for mkdir passed++++++\n");
	}
	else{
		printf("------test for mkdir failed------\n");
	}
}

int main(void){

	system("bash script_bis_rm.sh");
	system("bash script_bis.sh");

	makedir_test();

	system("bash script_bis_rm.sh");

  return 0;
}
