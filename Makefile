CC = gcc
EXEC = tsh
FLAGS = -g -Wall

SRC_DIR = src/
UTILS_DIR = src/utils/
TYPES_DIR = src/types/
TESTS_DIR = src/tests/

ALL_DIRS = $(SRC_DIR) $(TYPES_DIR) $(UTILS_DIR)
FILES = $(foreach dir, $(ALL_DIRS), $(wildcard $(dir)*.c))
OBJ = $(FILES:%.c=%.o)

%.o : %.c
	$(CC) -o $@ -c $< $(FLAGS)

$(EXEC) : $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(FLAGS)

.PHONY = clean
clean:
	rm $(OBJ) $(EXEC)

test:
	cd $(TESTS_DIR) && $(MAKE)

clean_test:
	cd $(TESTS_DIR) && $(MAKE) clean_all