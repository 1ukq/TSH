#include "init_header.h"

void set_checksum(struct posix_header *hd){
    memset(hd->chksum,' ',8);
    unsigned int sum = 0;
    char *p = (char *)hd;
    for (int i=0; i < BLOCK_SIZE; i++) { sum += p[i]; }
    sprintf(hd->chksum,"%06o",sum);
}

int check_checksum(struct posix_header *hd){
    unsigned int checksum;
    sscanf(hd->chksum,"%o ", &checksum);
    unsigned int sum = 0;
    char *p = (char *)hd;
    for (int i=0; i < BLOCK_SIZE; i++) { sum += p[i]; }
    for (int i=0;i<8;i++) { sum += ' ' - hd->chksum[i]; }
    return (checksum == sum);
}

void init_type_file(struct stat *restrict buf, struct posix_header *header){
    if(S_ISREG(buf -> st_mode)) (*header).typeflag = REGTYPE;
    else if(S_ISDIR(buf -> st_mode)) (*header).typeflag = DIRTYPE;
    else if(S_ISFIFO(buf -> st_mode)) (*header).typeflag = FIFOTYPE;
    else if(S_ISLNK(buf -> st_mode)) (*header).typeflag = LNKTYPE;
    else if(S_ISCHR(buf -> st_mode)) (*header).typeflag = CHRTYPE;
    else if(S_ISBLK(buf -> st_mode)) (*header).typeflag = BLKTYPE;
    else if(S_ISSOCK(buf -> st_mode)) (*header).typeflag = SOCKTYPE;
}

void init_mode_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> mode, "%s", "0000000\0");
    int usr = 0;
    int grp = 0;
    int oth = 0;
    if(buf -> st_mode & S_IRUSR) usr += 4;
    if(buf -> st_mode & S_IWUSR) usr += 2;
    if(buf -> st_mode & S_IXUSR) usr += 1;
    if(buf -> st_mode & S_IRGRP) grp += 4;
    if(buf -> st_mode & S_IWGRP) grp += 2;
    if(buf -> st_mode & S_IXGRP) grp += 1;
    if(buf -> st_mode & S_IROTH) oth += 4;
    if(buf -> st_mode & S_IWOTH) oth += 2;
    if(buf -> st_mode & S_IXOTH) oth += 1;
    (*header).mode[6] += oth;
    (*header).mode[5] += grp;
    (*header).mode[4] += usr;

}

void init_size_file(struct stat *restrict buf, struct posix_header *header){

    //sprintf(header -> size, "%011llo", buf -> st_size); //Line for MacOs
    sprintf(header -> size, "%011lo", buf -> st_size);

}

void init_uid_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> uid, "%7o", buf -> st_uid);
    for(int i = 0; i < 8; i++) if((*header).uid[i] == ' ') (*header).uid[i] = '0';

}

void init_gid_file(struct stat *restrict buf, struct posix_header *header){

    sprintf(header -> gid, "%7o", buf -> st_gid);
    for(int i = 0; i < 8; i++) if((*header).gid[i] == ' ') (*header).gid[i] = '0';

}

void init_mtime_file(struct stat *restrict buf, struct posix_header *header){

    //sprintf(header -> mtime, "%lo", buf -> st_mtimespec.tv_sec); //Line for MacOS
    sprintf(header -> mtime, "%lo", buf -> st_mtim.tv_sec);

}

void init_name_file(struct posix_header *header, const char *path_file_source, char *dest){

    char *str = name(path_file_source);
    char *path = concatenate(dest, str);
    sprintf(header -> name, "%s", path);

}

void init_magic_file(struct posix_header *header){

    sprintf(header -> magic, "%s", TMAGIC);

}

void init_version_file(struct posix_header *header){

    (*header).version[0] = ' ';
    (*header).version[1] = ' '; 

}

int create_file_header(const char *path_file_source, struct stat *restrict buf, struct posix_header *header, char *dest){

    init_name_file(header, path_file_source, dest);
    init_mode_file(buf, header);
    init_size_file(buf, header);
    init_type_file(buf, header);
    init_magic_file(header);
    init_version_file(header);
    init_uid_file(buf, header);
    init_gid_file(buf, header);
    init_mtime_file(buf, header);
    set_checksum(header);
    int n = check_checksum(header);
    if(!n){
        return -1;
    }
    return 0;

}