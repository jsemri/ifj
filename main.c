/*
 * IFJ 2016
 * FIT VUT Brno
 * IFJ16 Interpret Project
 *
 * Authors:
 * Jakub   Semric     - xsemri00
 * Peter   Rusinak    - xrusin03
 * Krystof Rykala     - xrykal00
 * Martin  Mikan      - xmikan00
 * Martin  Polakovic  - xpolak33
 *
 * Unless otherwise stated, all code is licenced under a
 * GNU General Public License v2.0
 *
 */

#include <stdio.h>
#include "parser.h"
#include "globals.h"

FILE *source;

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Error: No input file.\n");
        return INTERNAL_ERROR;
    }
    if (!(source = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: Opening the file failed.\n");
        return INTERNAL_ERROR;
    }

    int res = parse();

    fclose(source);

    return res;
}
