#include "move.h"

int find_last_block(int fd_tar){

    int size_tar = lseek(fd_tar, 0, SEEK_END);
    int ret = size_tar - (2 * BLOCK_SIZE);
    int ret_lseek = lseek(fd_tar, 0, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in find_last_block") == -1) return -1;
    return ret;

}

char *buffarize(const char *restrict path_file_source, struct stat *restrict buf){

    int fd_source = open(path_file_source, O_RDONLY);
    if(check_sys_call(fd_source, "open in buffarize") == -1) return NULL;

    off_t size = buf -> st_size;

    int nb_blocks = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    char *file_source_buf = malloc(sizeof(char) * BLOCK_SIZE * nb_blocks);
    memset(file_source_buf, '\0', BLOCK_SIZE * nb_blocks);
    if(file_source_buf == NULL){
        perror("malloc in buffarize");
        return NULL;
    }

    int nb_char_read = read(fd_source, file_source_buf, size);
    if(check_sys_call(nb_char_read, "read in buffarize") == -1) return NULL;

    return file_source_buf;

}

int insert_file_in_tar(const char *path_tar, const char *path_file_source, char *path_in_tar){

    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in insert_file_in_tar") == -1) return -2;

    //int lseek_ret = lseek(fd_tar, - 2 * BLOCK_SIZE, SEEK_END); //This line is for bsdtar not GNU tar (bsdtar is the default tar utility on MacOS)
    int b = find_last_block(fd_tar);
    int lseek_ret = lseek(fd_tar, b, SEEK_SET);
    if(check_sys_call(lseek_ret, "lseek in insert_file_in_tar") == -1) return -1;

    struct stat stat_buf_source;
    int check_stat = stat(path_file_source, &stat_buf_source);
    if(check_sys_call(check_stat, "stat in insert_file_in_tar") == -1) return -1;
    struct posix_header header;
    memset(&header, '\0', sizeof(char) * BLOCK_SIZE);

    create_file_header(path_file_source, &stat_buf_source, &header, path_in_tar);
    char *file_source_buf = buffarize(path_file_source, &stat_buf_source);

    int wr = write(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(wr, "write in insert_file_in_tar") == -1) return -1;

    int size = stat_buf_source.st_size;
    int nb_blocks = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;
    wr = write(fd_tar, file_source_buf, nb_blocks * BLOCK_SIZE);
    if(check_sys_call(wr, "write in insert_file_in_tar") == -1) return -1;

    char *null_buf = malloc(2 * BLOCK_SIZE);
    memset(null_buf, '\0', 2 * BLOCK_SIZE);
    wr = write(fd_tar, null_buf, 2 * BLOCK_SIZE);
    if(check_sys_call(wr, "write in insert_file_in_tar") == -1) return -1;

    free(file_source_buf);
    close(fd_tar);

    return 0;

}

int mv_from_tar_to_tar(const char *path_tar_source, const char *path_tar_target, const char *path_file_source, char *path_in_tar){

    int fd_source = open(path_tar_source, O_RDWR);
    if(check_sys_call(fd_source, "open in mv_from_tar_to_tar") == -1) return -2;

    int fd_target;

    if(strcmp(path_tar_source, path_tar_target)){
        fd_target = open(path_tar_target, O_RDWR);
        if(check_sys_call(fd_target, "open in mv_from_tar_to_tar") == -1) return -2;
    }
    else{
        fd_target = fd_source;
    }

    int size_tar = lseek(fd_source, 0, SEEK_END);
    if(size_tar == -1){
        perror("lseek in mv_from_tar_to_tar");
        return -1;
    }

    int pos = lseek(fd_source, 0, SEEK_SET);
    if(check_sys_call(pos, "lseek in mv_from_tar_to_tar") == -1) return -1;

    int size_read = 0;
    int size = 0;
    int shift = 0;
    int ret_lseek = 0;
    struct posix_header header;

    size_read = read(fd_source, &header, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in mv_from_tar_to_tar") == -1) return -1;

    while(strcmp(header.name, path_file_source)){

        if(header.name[0] == '\0'){
            return -2;
        }

        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

        ret_lseek = lseek(fd_source, shift * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_tar") == -1) return -1;

        size_read = read(fd_source, &header, BLOCK_SIZE);
        if(check_sys_call(size_read, "read in mv_from_tar_to_tar") == -1) return -1;

    }

    sscanf(header.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in mv_from_tar_to_tar");
        return -1;
    }
    memset(buf, '\0', shift * BLOCK_SIZE);

    ret_lseek = lseek(fd_source, -BLOCK_SIZE, SEEK_CUR);
    if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_tar") == -1) return -1;

    struct posix_header hd;

    size_read = read(fd_source, &hd, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in mv_from_tar_to_tar") == -1) return -1;

    char *str = name(path_file_source);
    char *path = concatenate(path_in_tar, str);
    memset(hd.name, '\0', 100);
    sprintf(hd.name, "%s", path);
    set_checksum(&hd);

    size_read = read(fd_source, buf, shift * BLOCK_SIZE);
    if(check_sys_call(size_read, "read in mv_from_tar_to_tar") == -1) return -1;

    int pos_from = ret_lseek;
    int pos_to = pos_from + (shift + 1) * BLOCK_SIZE;
    int ret_sup = suppress_file(fd_source, pos_from, pos_to, size_tar);
    if(ret_sup == -1){
        return -1;
    }

    struct stat stat_target;
    int check_stat = stat(path_tar_source, &stat_target);
    if(check_sys_call(check_stat, "stat in mv_from_tar_to_tar") == -1) return -1;

    ret_lseek = lseek(fd_target, 0, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_tar") == -1) return -1;
    int b = find_last_block(fd_target);
    ret_lseek = lseek(fd_target, b, SEEK_SET);
    if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_tar") == -1) return -1;

    int size_write = write(fd_target, &hd, BLOCK_SIZE);
    if(check_sys_call(size_write, "write in mv_from_tar_to_tar") == -1) return -1;

    size_write = write(fd_target, buf, shift * BLOCK_SIZE);
    if(check_sys_call(size_write, "write in mv_from_tar_to_tar") == -1) return -1;

    char *null_buf = malloc(2 * BLOCK_SIZE);
    memset(null_buf, '\0', 2 * BLOCK_SIZE);
    size_write = write(fd_target, null_buf, 2 * BLOCK_SIZE);
    if(check_sys_call(size_write, "write in mv_from_tar_to_tar") == -1) return -1;

    close(fd_source);
    close(fd_target);
    free(buf);

    return 0;

}

int mv_from_dir_to_tar(const char *path_tar, const char *path_file_source, char *path_in_tar){

    int n = 0;
    int rem = 0;
    n = insert_file_in_tar(path_tar, path_file_source, path_in_tar);
	rem = remove(path_file_source);
    if(check_sys_call(rem, "remove in mv_from_tar_to_tar") == -1) return -1;
    return n;

}

int mv_from_tar_to_dir(const char *path_tar, const char *path_file_source, char *path_dest){

    int fd_tar = open(path_tar, O_RDWR);
    if(check_sys_call(fd_tar, "open in mv_from_tar_to_dir") == -1) return -2;

    int size = 0;
    int shift = 0;
    int ret_lseek = 0;

    struct posix_header header;
    int size_read = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in mv_from_tar_to_dir") == -1) return -2;

    while(strcmp(header.name, path_file_source)){

        if(header.name[0] == '\0'){
            return -2;
        }

        sscanf(header.size, "%o", &size);
        shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

        ret_lseek = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
        if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_dir") == -1) return -1;

        size_read = read(fd_tar, &header, BLOCK_SIZE);
        if(check_sys_call(size_read, "read in mv_from_tar_to_dir") == -1) return -1;

    }

    sscanf(header.size, "%o", &size);
    shift = size % BLOCK_SIZE == 0 ? size / BLOCK_SIZE : (size / BLOCK_SIZE) + 1;

    char *buf = malloc(sizeof(char) * shift * BLOCK_SIZE);
    if(buf == NULL){
        perror("malloc in mv_from_tar_to_dir");
    }
    size_read = read(fd_tar, buf, sizeof(char) * shift * BLOCK_SIZE);
    if(check_sys_call(size_read, "read in mv_from_tar_to_dir") == -1) return -1;

    ret_lseek = lseek(fd_tar, -BLOCK_SIZE * (shift + 1), SEEK_CUR);
    if(check_sys_call(ret_lseek, "lseek in mv_from_tar_to_dir") == -1) return -1;

    int pos_from = ret_lseek;
    int pos_to = pos_from + (shift + 1) * BLOCK_SIZE;
    int size_tar = lseek(fd_tar, 0, SEEK_END);
    if(check_sys_call(size_tar, "lseek in mv_from_tar_to_dir") == -1) return -1;
    int sup = suppress_file(fd_tar, pos_from, pos_to, size_tar);
    if(sup == -1) return -1;

    char *str = name(header.name);
    char *path = concatenate(path_dest, str);

    mode_t mode;
    //sscanf(header.mode, "%ho", &mode); //Line for MacOS
    sscanf(header.mode, "%o", &mode);

    int fd_dest = open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
    if(check_sys_call(fd_dest, "open in mv_from_tar_to_dir") == -1) return -2;

    int size_write = write(fd_dest, buf, sizeof(char) * size);
    if(check_sys_call(size_write, "write in mv_from_tar_to_dir") == -1) return -1;

    free(str);
    free(path);
    close(fd_tar);

    return 0;

}

int mv(char * path_file_source, char * path_file_dest){
	int n, taille;

	// get path_to_tar_source & path_in_tar_source
	struct work_directory wd_source;
	char path_to_tar_source[sizeof(wd_source.c_htar) + 1 + sizeof(wd_source.tar_name)];
	char path_in_tar_source[sizeof(wd_source.c_tar)];

	fill_wd(path_file_source, &wd_source);
	sprintf(path_to_tar_source, "%s/%s", wd_source.c_htar, wd_source.tar_name);
	sprintf(path_in_tar_source, "%s", wd_source.c_tar);

	// get path_to_tar_dest & path_in_tar_dest
	struct work_directory wd_dest;
	char path_to_tar_dest[sizeof(wd_dest.c_htar) + 1 + sizeof(wd_dest.tar_name)];
	char path_in_tar_dest[sizeof(wd_dest.c_tar)];

	fill_wd(path_file_dest, &wd_dest);
	sprintf(path_to_tar_dest, "%s/%s", wd_dest.c_htar, wd_dest.tar_name);
	sprintf(path_in_tar_dest, "%s", wd_dest.c_tar);

	// get the right mv function
	if(strlen(wd_source.tar_name) == 0){
		if(strlen(wd_dest.tar_name) != 0){
			//source: non-tar -> dest: tar
			// on lance mv_from_dir_to_tar
			taille = strlen(path_file_source);
			if(path_file_source[taille-1] == '/'){
				path_file_source[taille-1] = '\0';
			}
			n = mv_from_dir_to_tar(path_to_tar_dest, path_file_source, path_in_tar_dest);

			return n;
		}
		else{
			//source: non-tar -> dest: non-tar
			// aucun chemin n'implique de tar
			return -2;
		}
	}
	else if(strlen(wd_source.tar_name) != 0){
		//on enlève le / à la fin de path_in_tar_source si il y en a un
		taille = strlen(path_in_tar_source);
		if(taille > 0 && path_in_tar_source[taille-1] == '/'){
			path_in_tar_source[taille-1] = '\0';
		}
		//on est dans le cas de déplacement d'un tar
		if(taille == 0){
			n = fork();
			switch (n) {
				case -1:
					perror("fork in mv");
					return -1;
				case 0:
					path_file_source[strlen(path_file_source)-1] = '\0'; // on enlève le dernier /
					execlp("mv", "mv", path_file_source, path_file_dest, NULL);
					return 0;
				default:
					wait(NULL);
					return 0;
			}
		}
		if(strlen(wd_dest.tar_name) == 0){
			//source: tar -> dest: non-tar
			// on lance mv_from_tar_to_dir
			taille = strlen(path_in_tar_source);
			if(taille > 0 && path_in_tar_source[taille-1] == '/'){
				path_in_tar_source[taille-1] = '\0';
			}
			n = mv_from_tar_to_dir(path_to_tar_source, path_in_tar_source, path_file_dest);

			return n;
		}
		else{
			//source: tar -> dest: tar
			// on lance mv_from_tar_to_tar
			taille = strlen(path_in_tar_source);
			if(taille > 0 && path_in_tar_source[taille-1] == '/'){
				path_in_tar_source[taille-1] = '\0';
			}
			n = mv_from_tar_to_tar(path_to_tar_source, path_to_tar_dest, path_in_tar_source, path_in_tar_dest);

			return n;
		}
	}
    return 0;
}
