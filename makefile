CC=gcc

COVERAGE_FLAGS= -std=c99 -Wall -Wextra -pedantic -fprofile-arcs -ftest-coverage -fPIC -O0
CFLAGS= -std=c99 -Wall -Wextra -pedantic -g
PROJ=ifj

SRC_FILES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJ_FILES=$(patsubst %.c, %.o, $(SRC_FILES))

.PHONY: all clean release pack pack_test

all: release

# gcovr -r . --html --html-details -o test-converage.html
coverage: CFLAGS = $(COVERAGE_FLAGS)
coverage: clean release

release: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $(PROJ)

clean:
	rm -f $(PROJ) $(OBJ_FILES) *.gcda *.gcno *.html
pack:
	zip xsemri00.zip *.c *.h makefile rozdeleni dokumentace.pdf

pack_test:
	zip -r xsemri00+tests.zip *.c *.h makefile testsuites error_detection_test interpret_tests

test:
	./error_detection_test; ./interpret_tests
