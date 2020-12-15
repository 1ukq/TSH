CC = gcc
FLAGS = -g -Wall

SRC_DIR = src/
UTILS_DIR = src/utils/
TYPES_DIR = src/types/
TESTS_DIR = src/tests/
MAIN_DIR = src/main/

ALL_DIRS = $(SRC_DIR) $(TYPES_DIR) $(UTILS_DIR) $(MAIN_DIR)
FILES = $(foreach dir, $(ALL_DIRS), $(wildcard $(dir)*.c))
OBJ = $(FILES:%.c=%.o)

EXEC_LS = ls_t
OBJ_LS = src/main/ls_main.o src/list_file.o src/utils/fonctions_utiles.o

EXEC_CAT = cat_t
OBJ_CAT = src/main/cat_main.o src/copy.o src/utils/find_file.c src/utils/fonctions_utiles.o src/utils/init_header.o src/utils/utils_string.o

EXEC_MKDIR = mkdir_t
OBJ_MKDIR = src/main/mkdir_main.o src/makedir.o src/utils/init_header.o src/utils/fonctions_utiles.o src/utils/utils_string.o

EXEC_MV = mv_t
OBJ_MV = src/main/mv_main.o src/move.o src/utils/init_header.o src/utils/fonctions_utiles.o src/utils/utils_string.o src/utils/find_file.c

EXEC_PWD = pwd_t
OBJ_PWD = src/main/pwd_main.o src/pwd.o

EXEC_RM = rm_t
OBJ_RM = src/main/rm_main.o src/remove.o src/utils/find_file.o src/utils/fonctions_utiles.o

EXEC_CP = cp_t
OBJ_CP = src/main/cp_main.o src/copy.o src/utils/find_file.o src/utils/init_header.o src/utils/utils_string.o

%.o : %.c
	$(CC) -o $@ -c $< $(FLAGS)

$(EXEC_LS) : $(OBJ_LS)
	$(CC) -o $@ $^ $(FLAGS)
	mv $(EXEC_LS) src/execs

$(EXEC_CAT) : $(OBJ_CAT)
	$(CC) -o $@ $^ $(FLAGS)
	mv $(EXEC_CAT) src/execs

$(EXEC_MKDIR) : $(OBJ_MKDIR)
	$(CC) -o $@ $^ $(FLAGS)
	mv $(EXEC_MKDIR) src/execs

$(EXEC_MV) : $(OBJ_MV)
	$(CC) -o $@ $^ $(FLAGS)
	mv $(EXEC_MV) src/execs

$(EXEC_CP) : $(OBJ_CP)
	$(CC) -o $@ $^ $(FLAGS)
	mv $(EXEC_CP) src/execs

all : $(OBJ)
	$(CC) -o $(EXEC_LS) $(OBJ_LS) $(FLAGS)
	mv $(EXEC_LS) src/execs
	$(CC) -o $(EXEC_CAT) $(OBJ_CAT) $(FLAGS)
	mv $(EXEC_CAT) src/execs
	$(CC) -o $(EXEC_MV) $(OBJ_MV) $(FLAGS)
	mv $(EXEC_MV) src/execs
	$(CC) -o $(EXEC_PWD) $(OBJ_PWD) $(FLAGS)
	mv $(EXEC_PWD) src/execs
	$(CC) -o $(EXEC_MKDIR) $(OBJ_MKDIR) $(FLAGS)
	mv $(EXEC_MKDIR) src/execs
	$(CC) -o $(EXEC_RM) $(OBJ_RM) $(FLAGS)
	mv $(EXEC_RM) src/execs
	rm $(OBJ)

.PHONY = clean
clean:
	rm $(OBJ) src/execs/ls_t src/execs/cat_t src/execs/mv_t src/execs/pwd_t src/execs/mkdir_t src/execs/rm_t


test:
	cd $(TESTS_DIR) && $(MAKE)

clean_test:
	cd $(TESTS_DIR) && $(MAKE) clean_all