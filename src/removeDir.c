#include "removeDir.h"


int removeDirectory(const char *path_tar, const char *path_file_source){

  int fd_tar = open(path_tar, O_RDWR);
  if(check_sys_call(fd_tar, "open in remove_file_from_tar") == -1) return -2;

  int size_tar = lseek(fd_tar, 0, SEEK_END);
  if(check_sys_call(size_tar, "lseek in remove_file_from_tar") == -1) return -1;

  int pos = lseek(fd_tar, 0, SEEK_SET);
  if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

  struct posix_header header;
  ssize_t size_read = read(fd_tar, &header, BLOCK_SIZE);
  if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;

  int pos_dir = 0;
  int file_size = 0;
  int shift = 0;

  while((header.name[0] == '\0')){
    /*je regarde combien de fois le nom du dossier désiré est présent. Si il l'est plus d'une fois il n'est pas vide. Je retiens la position de la première occurence du dossier,
    car si il y en a une deuxième il y a erreur. Et cette position me permettra de déplacerle reste du tar afin de supprimer le dossier*/
    if(strncmp(header.name, path_file_source, strlen(path_file_source)) == 0){
      count++;
      //directory non vide
      if(count > 1) return -2;
      pos_dir = pos - BLOCK_SIZE;
    }
    sscanf(header.size, "%o", &file_size)
    //le shift pour passer au bloc suivant
    shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;
    pos = lseek(fd_tar, shift * BLOCK_SIZE, SEEK_CUR);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;
    //puis je lis le header pour la prochaine itération du while
    size_read = read(fd_tar, &header, BLOCK_SIZE);
    if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;
  }
  //Si il y a exactement une occurence du dossier
  if(count != 0){
    //j'ai repris le code de Lucas, car le principe pour supprimer un dossier et un fichier est le même à partir de là.
    sscanf(header.size, "%o", &file_size);
    shift = file_size % BLOCK_SIZE == 0 ? file_size / BLOCK_SIZE : (file_size / BLOCK_SIZE) + 1;

    pos = lseek(fd_tar, -BLOCK_SIZE, SEEK_CUR);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    char *buf_write = malloc(sizeof(char) * (size_tar - pos));
    memset(buf_write, '\0', sizeof(char) * (size_tar - pos));

    int n = pos;

    pos = lseek(fd_tar, BLOCK_SIZE * (shift + 1), SEEK_CUR);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    size_read = read(fd_tar, buf_write, (size_tar - n));
    if(check_sys_call(size_read, "read in remove_file_from_tar") == -1) return -1;


    pos = lseek(fd_tar, n, SEEK_SET);
    if(check_sys_call(pos, "lseek in remove_file_from_tar") == -1) return -1;

    int size_write = write(fd_tar, buf_write, size_tar - n);
    if(check_sys_call(size_write, "write in remove_file_from_tar") == -1) return -1;

    close(fd_tar);
    free(buf_write);
    return 0;
  }
}
