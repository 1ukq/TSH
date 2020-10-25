CC = gcc
EXEC = tests
FLAGS = -g -Wall

MUNIT_C_FILE = munit/munit.c
SRC = ../copy.c ../list_file.c ../makedir.c ../remove.c

$(EXEC) : tests.c $(MUNIT_C_FILE) $(SRC)
	$(CC) $^ -o $(EXEC)

.PHONY = clean
clean :
	rm *.o $(EXEC)