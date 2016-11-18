#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include <ctype.h>
#include "token.h"
#include "string.h"
#include "globals.h"
#include "debug.h"
#include "symbol.h"
#include <stdlib.h>

#define KEYW_COUNT 17

FILE *source;
T_token *token;
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

#define return return show_token(0) +
#define SIZE 1024
int get_token() {
    T_state state = S_start;
    int c; //read character
    token_clear(token);
    char buf[SIZE] = "";
    unsigned counter = 0;

    // Reads chars from a file until EOF occurs
    while((c = fgetc(source)) != EOF) {
        switch (state) {
            case S_start:
                if (isspace(c)) {
                    ;    // Ignores whitespaces, do nothing
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
                    buf[counter++] = c;
                    state = S_ID;
                } else if (c == '"') {
                    state = S_string;
                } else if (isdigit(c)) {
                    state = S_int;
                    buf[counter++] = c;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            /*_________Numbers________*/
            case S_int:
                if (isdigit(c)) {
                    buf[counter++] = c;     // add number to a string
                } else if (c == '.') {
                    state = S_double;
                    buf[counter++] = c;     // add the dot into the string
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    buf[counter++] = c;
                } else {
                    ungetc(c, source);          // last char was not valid, end of ID, undo last char

                    token->type = TT_int;
                    token->attr.n = (int) strtol(buf, NULL, 10); //return string value in int


                    return 0;
                }
                break;

            case S_double:
                if (isdigit(c)) {
                    state = S_double2;
                    buf[counter++] = c;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_double2:
                if (isdigit(c)) {
                    buf[counter++] = c; // add into string and stay
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    buf[counter++] = c;
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(buf, NULL); //return string value in int

                    return 0;
                }
                break;

            case S_doubleExp:
                if (c == '+' || c == '-') {
                    state = S_doubleExp2;
                    buf[counter++] = c;
                } else if (isdigit(c)) {
                    state = S_doubleExp3;
                    buf[counter++] = c;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_doubleExp2:
                if (isdigit(c)) {
                    state = S_doubleExp3;
                    buf[counter++] = c;
                } else {
                    terminate(LEX_ERROR);
                }
                break;

            case S_doubleExp3:
                if (isdigit(c)) {
                    buf[counter++] = c; // add into string and stay in this state
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(buf, NULL);


                    return 0;
                }
                break;

            /*_________String________*/
            case S_string:
                if (c == '\\') {
                    state = S_stringBackSlash;
                } else if (c == '"') {
                    token->type = TT_string;
                    token->attr.str = get_str(buf);
                    return 0;
                } else if (c == '\n' || c == EOF) {
                    terminate(LEX_ERROR);
                } else {
                    buf[counter++] = c;
                }
                break;

            case S_stringBackSlash:
                if (c == '\n' || c == EOF) {
                    terminate(LEX_ERROR);
                } else {
                    state = S_string;
                }
                break;

            /*_________ID________*/
            case S_ID:
                if (isalnum(c) || c == '_' || c == '$') {
                    buf[counter++] = c;     // add char to str, the ID is valid
                }
                else if (c == '.') {
                    int i;
                    // checking if id is keyword
                    for (i = 0; i < KEYW_COUNT &&
                            strcmp(buf, KEYWORDS[i]); i++);
                    if (i != KEYW_COUNT) {
                        terminate(LEX_ERROR);
                    }

                    buf[counter++] = c;
                    state = S_FULL_ID;
                }
                else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char
                    for (int i = 0; i < KEYW_COUNT; i++) {
                        if (strcmp(buf, KEYWORDS[i]) == 0) {
                            token->type = TT_keyword;
                            token->attr.keyword = i;
                            return 0;
                        }
                    }
                    token->type = TT_id;
                    token->attr.str = get_str(buf);
                    return 0;
                }
                break;
            /*________FULL ID________*/
            case S_FULL_ID:
                if (isalnum(c) || c == '_' || c== '$') {
                    buf[counter++] = c;
                }
                else if (buf[counter-1] == '.' && isspace(c)) {
                    // space between dot and identifiers
                    terminate(LEX_ERROR);
                }
                else
                {
                    ungetc(c, source);
                    char *ptr = strchr(buf, '.');
                    ptr++;
                    int i;
                    // checking if id is keyword
                    for (i = 0; i < KEYW_COUNT &&
                            strcmp(ptr, KEYWORDS[i]); i++);
                    if (i != KEYW_COUNT) {
                        terminate(LEX_ERROR);
                    }

                    token->attr.str = get_str(buf);
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
                } else {                    // it is division
                    token->type = TT_div;
                    ungetc(c, source);      // we need to return that one char we checked
                    return 0;
                }
                break;

            case S_commentLine:
                if (c == '\n') {
                    state = S_start;
                }
                break;

            case S_commentBlock:
                if (c == '*') {
                    state = S_commentBlockStar;
                } else if (is_next_eof()) {
                    token->type = TT_eof;
                    return 0;
                }
                break;

            case S_commentBlockStar:
                if (c == '*') {
                    ;
                } else if (c == '/') {
                    state = S_start;
                } else if (is_next_eof()) {
                    token->type = TT_eof;
                    return 0;
                } else {
                    state = S_commentBlock;
                }
                break;
        }
    }
    // Pokud funkce dojde az, sem musel byt nacteny charakter EOF
    token->type = TT_eof;
    return 0;
}
