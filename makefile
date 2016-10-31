CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic -g # -DDEBUG
PROJ=ifj

SRC_FILES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

.PHONY: all clean

all: release

release: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJ)

clean:
	rm -f $(PROJ) $(OBJ_FILES)
#main: main.c globals.h parser.o
#	$(CC) $(CFLAGS) parser.o main.c -o $@

#parser.o: parser.c parser.h token.o  token_vector.o globals.h scanner.o string.o
#	$(CC) $(CFLAGS) -c scanner.o token.o string.o token_vector.o parser.c -o $@

#scanner.o: scanner.c scanner.h token.o string.o globals.h
#	$(CC) $(CFLAGS) -c token.o string.o scanner.c -o $@

#string.o: string.h string.c
#	$(CC) $(CFLAGS) -c string.c -o $@

#token.o: token.h token.c string.o
#	$(CC) $(CFLAGS) -c string.o token.c -o $@

#token_vector.o: token_vector.h token_vector.c token.o string.o
#	$(CC) $(CFLAGS) -c string.o token.o token_vector.c -o $@

