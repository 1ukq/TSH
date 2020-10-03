CC = gcc
EXEC = tsh
FLAGS = -g -Wall

SRC_DIR = src/
TYPES_DIR = src/types/
TESTS_DIR = src/tests/

ALL_DIRS = $(SRC_DIR) $(TYPES_DIR) $(TESTS_DIR)
FILES = $(foreach dir, $(ALL_DIRS), $(wildcard $(dir)*.c))
OBJ = $(FILES:%.c=%.o)

%.o : %.c
	$(CC) -o $@ -c $< $(FLAGS)

$(EXEC) : $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(FLAGS)

.PHONY = clean
clean:
	rm $(OBJ) $(EXEC)