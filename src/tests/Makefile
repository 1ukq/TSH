CC = gcc
FLAGS = -g -Wall

SRC_DIR = ../
UTILS_DIR = ../utils/
TYPES_DIR = ../types/
TESTS_DIR = ./

MUNIT_FILE = munit/munit.c
MUNIT_OBJ = munit/munit.o

ALL_DIRS = $(SRC_DIR) $(TYPES_DIR) $(UTILS_DIR) $(TESTS_DIR)
FILES = $(foreach dir, $(ALL_DIRS), $(wildcard $(dir)*.c)) $(MUNIT_FILE)
OBJ = $(FILES:%.c=%.o)
EXECS = move_tests copy_tests remove_tests ls_tests cd_tests makedir_tests pwd_tests

MOVE_OBJ = move_tests.o ../utils/init_header.o ../move.o ../utils/utils_string.o ../utils/fonctions_utiles.o ../utils/find_file.o $(MUNIT_OBJ)
REMOVE_OBJ = remove_tests.o ../remove.o $(MUNIT_OBJ)
COPY_OBJ = copy_tests.o ../copy.o $(MUNIT_OBJ)
LS_OBJ = ls_tests.o ../list_file.o ../utils/fonctions_utiles.o $(MUNIT_OBJ)
CD_OBJ = cd_tests.o ../cd.o ../utils/fonctions_utiles.o $(MUNIT_OBJ)
MAKEDIR_OBJ = makedir_tests.o ../makedir.o ../utils/fonctions_utiles.o ../utils/init_header.o ../utils/utils_string.o $(MUNIT_OBJ)
PWD_OBJ = pwd_tests.o ../pwd.o $(MUNIT_OBJ)

%.o : %.c
	@echo Compiling $?
	$(CC) -o $@ -c $< $(FLAGS)

move_tests : $(MOVE_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

remove_tests : $(REMOVE_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

copy_tests : $(COPY_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

ls_tests : $(LS_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

cd_tests : $(CD_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

makedir_tests : $(MAKEDIR_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

pwd_tests : $(PWD_OBJ)
	$(CC) -o $@ $^ $(FLAGS)

all : $(OBJ) $(MUNIT_OBJ)
	@echo Compiling tests...
	$(CC) -o move_tests $(MOVE_OBJ)
	$(CC) -o remove_tests $(REMOVE_OBJ)
	$(CC) -o copy_tests $(COPY_OBJ)
	$(CC) -o ls_tests $(LS_OBJ)
	$(CC) -o cd_tests $(CD_OBJ)
	$(CC) -o makedir_tests $(MAKEDIR_OBJ)
	$(CC) -o pwd_tests $(PWD_OBJ)

clean_move :
	rm $(MOVE_OBJ) move_tests

clean_remove :
	rm $(REMOVE_OBJ) remove_tests

clean_copy :
	rm $(COPY_OBJ) copy_tests

clean_ls :
	rm $(LS_OBJ) ls_tests

clean_cd :
	rm $(CD_OBJ) cd_tests

clean_makedir :
	rm $(MAKEDIR_OBJ) makedir_tests

clean_pwd :
	rm $(PWD_OBJ) pwd_tests

clean_all :
	@echo Removing object files and executables
	rm $(OBJ) $(EXECS)
