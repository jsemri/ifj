CC=gcc

COVERAGE_FLAGS= -std=c99 -Wall -Wextra -pedantic -fprofile-arcs -ftest-coverage -fPIC -O0
CFLAGS= -std=c99 -Wall -Wextra -pedantic -g
PROJ=ifj

SRC_FILES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

.PHONY: all clean

all: release

# gcovr -r . --html --html-details -o test-converage.html
coverage: CFLAGS = $(COVERAGE_FLAGS)
coverage: clean release

release: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJ)

clean:
	rm -f $(PROJ) $(OBJ_FILES)

