CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -pedantic -g  #-DDEBUG_TABLES #-DREC_DEBUG #-DSHOW_TOKEN
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

