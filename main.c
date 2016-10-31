#include <stdio.h>
#include "parser.h"
#include "globals.h"

FILE *source;

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Error: No input file.\n");
        return INPUT_ERROR;
    }
    if (!(source = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: Opening the file failed.\n");
        return INTERNAL_ERROR;
    }

    int res = parse();

    fclose(source);

    return res;
}
