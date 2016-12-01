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
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include <ctype.h>
#include "token.h"
#include <string.h>
#include "globals.h"
#include "debug.h"
#include "symbol.h"

#define KEYW_COUNT 17
#define is_oct(c)  (c >= '0' && c<= '7' )

FILE *source;
T_token *token;
int row;
// char vector
char *char_vector;
// vector max size
int vector_size;
// actual size
int len;

void push_char(int c) {
    if (len >= vector_size) {
        void *ptr = realloc(char_vector,(vector_size *= 2));
        if (!ptr)
            terminate(99);
        char_vector = ptr;
        memset(char_vector + len, 0, vector_size - len);
    }
    char_vector[len++] = c;
}

void clear() {
    len = 0;
    memset(char_vector, 0, vector_size);
}

#define TRUE_POS 15
#define FALSE_POS 10

const char *KEYWORDS[KEYW_COUNT] = {
    "void", "int", "double", "String", "boolean",
    "break", "class", "continue", "do", "else",
    "false", "for", "if", "return", "static", "true",
    "while" };

// sets a file to read from
void set_file(FILE *f) {
    source = f;
}

int is_next_eof() {
    int c = fgetc(source);    //checking EOF
    int ret_value = (c == EOF); // if next char
    ungetc(c, source);

    return ret_value;
}

int get_token() {
    T_state state = S_start;
    int c; //read character
    token_clear(token);
    clear();
    int b1, b2; // for octal numbers
    b1 = 0; b2 = 0;
    // Reads chars from a file until EOF occurs
    while((c = fgetc(source)) != EOF) {
        switch (state) {
            case S_start:
                if (isspace(c)) {
                    if (c == '\n')
                        row++;
                } else if (c == '+') {
                    token->type = TT_plus;
                    return 0;
                } else if (c == '-') {
                    token->type = TT_minus;
                    return 0;
                } else if (c == '*') {
                    token->type = TT_mul;
                    return 0;
                } else if (c == ',') {
                    token->type = TT_comma;
                    return 0;
                } else if (c == ';') {
                    token->type = TT_semicolon;
                    return 0;
                } else if (c == '(') {
                    token->type = TT_lBracket;
                    return 0;
                } else if (c == ')') {
                    token->type = TT_rBracket;
                    return 0;
                } else if (c == '{') {
                    token->type = TT_lCurlBracket;
                    return 0;
                } else if (c == '}') {
                    token->type = TT_rCurlBracket;
                    return 0;
                } else if (c == '/') {
                    if (is_next_eof()) {
                        token->type = TT_div;
                        return 0;
                    }
                    state = S_commentOrDiv;                 // comment (block, line) or division
                } else if (c == '<') {
                    state = S_lesserOrLesserEqual;          // < or <=
                } else if (c == '>') {
                    state = S_greaterOrGreaterEqual;        // > or >=
                } else if (c == '=') {
                    state = S_assignOrEqual;
                } else if (c == '!') {
                    state = S_notOrNotEqual;
                } else if (c == '|') {
                    state = S_or;
                } else if (c == '&'){
                    state = S_and;
                } else if (isalpha(c) || c == '_' || c == '$') {    // ID starts with _$a-Z
                    push_char(c);
                    state = S_ID;
                } else if (c == '"') {
                    state = S_string;
                } else if (isdigit(c)) {
                    state = S_int;
                    push_char(c);
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            /*_________Numbers________*/
            case S_int:
                if (isdigit(c)) {
                    push_char(c);     // add number to a string
                } else if (c == '.') {
                    state = S_double;
                    push_char(c);     // add the dot into the string
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    push_char(c);
                } else {
                    ungetc(c, source);          // last char was not valid, end of ID, undo last char

                    token->type = TT_int;
                    token->attr.n = (int) strtol(char_vector, NULL, 10);


                    return 0;
                }
                break;

            case S_double:
                if (isdigit(c)) {
                    state = S_double2;
                    push_char(c);
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_double2:
                if (isdigit(c)) {
                    push_char(c); // add into string and stay
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    push_char(c);
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(char_vector, NULL); //return string value in int

                    return 0;
                }
                break;

            case S_doubleExp:
                if (c == '+' || c == '-') {
                    state = S_doubleExp2;
                    push_char(c);
                } else if (isdigit(c)) {
                    state = S_doubleExp3;
                    push_char(c);
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_doubleExp2:
                if (isdigit(c)) {
                    state = S_doubleExp3;
                    push_char(c);
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_doubleExp3:
                if (isdigit(c)) {
                    push_char(c); // add into string and stay in this state
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(char_vector, NULL);


                    return 0;
                }
                break;

            /*_________String________*/
            case S_string:
                if (is_next_eof())
                    terminate(LEX_ERROR);

                if (c == '\\') {
                    state = S_stringBackSlash;
                }
                else if (c == '"') {
                    token->type = TT_string;
                    token->attr.str = get_str(char_vector);
                    return 0;
                }
                else if (c == '\n') {
                    row++;
                    terminate(LEX_ERROR);
                }
                else {
                    push_char(c);
                }
                break;

            case S_stringBackSlash:
                if (is_next_eof())
                    terminate(LEX_ERROR);

                if (c == '\n') {
                    row++;
                    terminate(LEX_ERROR);
                }
                else if (c == 'n') {
                    push_char('\n');
                    state = S_string;
                }
                else if (c == '"') {
                    push_char('"');
                    state = S_string;
                }
                else if (c == 't'){
                    push_char('\t');
                    state = S_string;
                }
                else if (c == '\\') {
                    push_char('\\');
                    state = S_string;
                }
                else if (c <= '3' && c>= '0') {
                    b1 = c - '0';
                    state = S_octal1;
                }
                else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_octal1:
                if (is_next_eof())
                    terminate(LEX_ERROR);

                if (is_oct(c)) {
                    b2 = c - '0';
                    state = S_octal2;
                }
                else
                    terminate(LEX_ERROR);
                break;

            case S_octal2:
                if (is_next_eof())
                    terminate(LEX_ERROR);

                if (is_oct(c)) {
                    c = c - '0' + (b1*64) + (b2*8);
                    // \ddd <001, 377>
                    if (c == 0 || c > 255)
                        terminate(1);
                    push_char(c);
                    state = S_string;
                }
                else
                    terminate(LEX_ERROR);

                break;

            /*_________ID________*/
            case S_ID:
                if (isalnum(c) || c == '_' || c == '$') {
                    push_char(c);     // add char to str, the ID is valid
                }
                else if (c == '.') {
                    int i;
                    // checking if id is keyword
                    for (i = 0; i < KEYW_COUNT &&
                            strcmp(char_vector, KEYWORDS[i]); i++);
                    if (i != KEYW_COUNT) {
                        terminate(LEX_ERROR);
                    }

                    push_char(c);
                    state = S_FULL_ID;
                }
                else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char
                    for (int i = 0; i < KEYW_COUNT; i++) {
                        if (strcmp(char_vector, KEYWORDS[i]) == 0) {
                            if (i == TRUE_POS || i == FALSE_POS) {
                                // boolean value found
                                token->type = TT_bool;
                                token->attr.b = (i == TRUE_POS);
                                return 0;
                            }
                            token->type = TT_keyword;
                            token->attr.keyword = i;
                            return 0;
                        }
                    }
                    token->type = TT_id;
                    token->attr.str = get_str(char_vector);
                    return 0;
                }
                break;
            /*________FULL ID________*/
            case S_FULL_ID:
                if (isalnum(c) || c == '_' || c== '$') {
                    push_char(c);
                }
                else if (char_vector[len-1] == '.' && isspace(c)) {
                    // space between dot and identifiers
                    terminate(LEX_ERROR);
                }
                else
                {
                    ungetc(c, source);
                    char *ptr = strchr(char_vector, '.');
                    ptr++;
                    int i;
                    // checking if id is keyword
                    for (i = 0; i < KEYW_COUNT &&
                            strcmp(ptr, KEYWORDS[i]); i++);
                    if (i != KEYW_COUNT) {
                        terminate(LEX_ERROR);
                    }

                    token->attr.str = get_str(char_vector);
                    token->type = TT_fullid;
                    return 0;
                }
                break;
            /*_________OPERATORS________*/
            case S_lesserOrLesserEqual:
                if (c == '=') {
                    token->type = TT_lessEq;
                    return 0;
                } else {
                    ungetc(c, source);            // It's just <, undo last char read
                    token->type = TT_lesser;
                    return 0;
                }
                break;

             case S_greaterOrGreaterEqual:
                if (c == '=') {
                    token->type = TT_greatEq;
                    return 0;
                } else {
                    ungetc(c, source);            // It's just >, undo last char read
                    token->type = TT_greater;
                    return 0;
                }
                break;

            case S_assignOrEqual:
                if (c == '=') {
                    token->type = TT_equal;
                    return 0;
                } else {
                    ungetc(c, source);            // Just =, undo last char
                    token->type = TT_assign;
                    return 0;
                }
                break;

            case S_notOrNotEqual:
                if (c == '=') {
                    token->type = TT_notEq;
                    return 0;
                } else {
                    ungetc(c, source);            // Just =, undo last char
                    token->type = TT_not;
                    return 0;
                }
                break;

            case S_or:
                if (c == '|') {
                    token->type = TT_or;
                    return 0;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_and:
                if (c == '&') {
                    token->type = TT_and;
                    return 0;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            /*_________COMMENTS_________*/
            case S_commentOrDiv:
                if (c == '/') {
                    state = S_commentLine;
                } else if (c == '*') {
                    state = S_commentBlock;
                    if (is_next_eof())
                        terminate(1);
                } else {                    // it is division
                    token->type = TT_div;
                    ungetc(c, source);      // we need to return that one char we checked
                    return 0;
                }
                break;

            case S_commentLine:
                if (c == '\n') {
                    row++;
                    state = S_start;
                }
                break;

            case S_commentBlock:
                if (c == '*') {
                    state = S_commentBlockStar;
                    if (is_next_eof())
                        terminate(1);
                }
                else if (is_next_eof()) {
                    terminate(LEX_ERROR);
                }
                break;

            case S_commentBlockStar:
                if (c == '*') {
                    if (is_next_eof())
                        terminate(1);
                }
                else if (c == '/') {
                    state = S_start;
                }
                else if (is_next_eof()) {
                    terminate(LEX_ERROR);
                }
                else {
                    row = c == '\n' ? row + 1: row;
                    state = S_commentBlock;
                }
                break;
        }
    }
    // Pokud funkce dojde az, sem musel byt nacteny charakter EOF
    token->type = TT_eof;
    return 0;
}
