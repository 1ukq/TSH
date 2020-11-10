CC = gcc
EXEC = tests
FLAGS = -g -Wall

SRC_DIR = ../
UTILS_DIR = ../utils/

MUNIT_C_FILE = munit/munit.c
UTILS_FILES = $(foreach dir, $(UTILS_DIR), $(wildcard $(dir)*.c))
SRC = ../copy.c ../list_file.c ../makedir.c ../remove.c ../init_header.c ../move.c ../cd.c ../pwd.c
FILE_TEST = tests.c
ALL_FILES = $(SRC) $(MUNIT_C_FILE) $(FILE_TEST) $(UTILS_FILES)
OBJ = $(ALL_FILES:%.c=%.o)

%.o : %.c
	$(CC) -o $@ -c $< $(FLAGS)

$(EXEC) : $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(FLAGS)

.PHONY = clean
clean :
	rm $(OBJ) $(EXEC)