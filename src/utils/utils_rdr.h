#include <string.h>
#include "fonctions_utiles.h"

#define STDIN 5 // < file
#define STDOUT_1 2 // > file
#define	STDOUT_2 1 // >> file
#define	STDERR_1 4 // 2> file
#define STDERR_2 3 // 2>> file

#define STDIN_TAR 10	// < tar_file
#define STDOUT_1_TAR 7 // > tar_file
#define STDOUT_2_TAR 6 // >> tar_file
#define STDERR_1_TAR 9	// 2> tar_file
#define STDERR_2_TAR 8	// 2>> tar_file

int nb_rdr(char * string);

char * rdr_file(char * string, char * cwd);

int rdr_type(char * string, char * cwd);
