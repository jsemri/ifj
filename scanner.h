#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "token.h"

#define OK 0

typedef enum {
    S_start,                    //initial
    S_commentOrDiv,             // first occurence of /, we need to decide if it's comment or divison
    S_commentBlock,
    S_commentBlockStar,
    S_commentLine,              // beggining of block comment /*
    S_lesserOrLesserEqual,      // < or <=
    S_greaterOrGreaterEqual,    // > or >=
    S_assignOrEqual,            // = or ==
    S_or,                       // ||
    S_and,                      // &&
    S_string,
    S_stringBackSlash,
    S_int,
    S_double,
    S_double2,
    S_doubleExp,
    S_doubleExp2,
    S_doubleExp3,
    S_notOrNotEqual,
    S_ID,
    S_FULL_ID,
    S_octal1,
    S_octal2
} T_state;

void set_file(FILE *f);
int get_token();

#endif
