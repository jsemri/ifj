#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include <ctype.h>
#include "token.h"
#include "string.h"
#include "globals.h"

FILE *source;
T_token *token;
const char *KEYWORDS[17] = { "boolean", "break", "class", "continue", "do", "double", "else", "false", "for", "if", "int", "return", "String", "static", "true", "void", "while" };

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
    T_string *number;

    // Reads chars from a file until EOF occurs
    while((c = fgetc(source)) != EOF) {
        switch (state) {
            case S_start:
                if (isspace(c)) {
                    ;    // Ignores whitespaces, do nothing
                } else if (c == '+') {
                    token->type = TT_plus;
                    return OK;
                } else if (c == '-') {
                    token->type = TT_minus;
                    return OK;
                } else if (c == '*') {
                    token->type = TT_mul;
                    return OK;
                } else if (c == '.') {
                    token->type = TT_dot;
                    return OK;
                } else if (c == ',') {
                    token->type = TT_comma;
                    return OK;
                } else if (c == ';') {
                    token->type = TT_semicolon;
                    return OK;
                } else if (c == '(') {
                    token->type = TT_lBracket;
                    return OK;
                } else if (c == ')') {
                    token->type = TT_rBracket;
                    return OK;
                } else if (c == '{') {
                    token->type = TT_lCurlBracket;
                    return OK;
                } else if (c == '}') {
                    token->type = TT_rCurlBracket;
                    return OK;
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
                    token->attr.str = str_init();
                    str_addchar(token->attr.str, c);
                    state = S_ID;
                } else if (c == '"') {
                    state = S_string;
                    token->attr.str = str_init();
                } else if (isdigit(c)) {
                    state = S_int;
                    number = str_init();
                    str_addchar(number, c);
                } else {
                    return LEX_ERROR;
                }
                break;

            /*_________Numbers________*/
            case S_int:
                if (isdigit(c)) {
                    str_addchar(number, c);     // add number to a string
                } else if (c == '.') {
                    state = S_double;
                    str_addchar(number, c);     // add the dot into the string
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    str_addchar(number, c);
                } else {
                    ungetc(c, source);          // last char was not valid, end of ID, undo last char

                    token->type = TT_int;
                    token->attr.n = (int) strtol(number->buf, NULL, 10); //return string value in int
                    str_free(number);

                    return OK;
                }
                break;

            case S_double:
                if (isdigit(c)) {
                    state = S_double2;
                    str_addchar(number, c);
                } else {
                    return LEX_ERROR;
                }
                break;

            case S_double2:
                if (isdigit(c)) {
                    str_addchar(number, c); // add into string and stay
                } else if (c == 'e' || c == 'E') {
                    state = S_doubleExp;
                    str_addchar(number, c);
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(number->buf, NULL); //return string value in int
                    str_free(number);

                    return OK;
                }
                break;

            case S_doubleExp:
                if (c == '+' || c == '-') {
                    state = S_doubleExp2;
                    str_addchar(number, c);
                } else if (isdigit(c)) {
                    state = S_doubleExp3;
                    str_addchar(number, c);
                } else {
                    return LEX_ERROR;
                }
                break;

            case S_doubleExp2:
                if (isdigit(c)) {
                    state = S_doubleExp3;
                    str_addchar(number, c);
                } else {
                    return LEX_ERROR;
                }
                break;

            case S_doubleExp3:
                if (isdigit(c)) {
                    str_addchar(number, c); // add into string and stay in this state
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char

                    token->type = TT_double;
                    token->attr.d = strtod(number->buf, NULL);
                    str_free(number);

                    return OK;
                }
                break;

            /*_________String________*/
            case S_string:
                if (c == '\\') {
                    state = S_stringBackSlash;
                } else if (c == '"') {
                    token->type = TT_string;
                    return OK;
                } else if (c == '\n' || c == EOF) {
                    return LEX_ERROR;
                } else {
                    str_addchar(token->attr.str, c);
                }
                break;

            case S_stringBackSlash:
                if (c == '\n' || c == EOF) {
                    return LEX_ERROR;
                } else {
                    state = S_string;
                }
                break;

            /*_________ID________*/
            case S_ID:
                if (isalnum(c) || c == '_' || c == '$') {
                    str_addchar(token->attr.str, c);     // add char to str, the ID is valid
                } else {
                    ungetc(c, source);            // last char was not valid, end of ID, undo last char
                    for (int i = TK_boolean; i <= TK_while; i++) {
                        if (strcmp(token->attr.str->buf, KEYWORDS[i]) == 0) {
                            token_clear(token);
                            token->type = TT_keyword;
                            token->attr.keyword = i;
                            return OK;
                        }
                    }
                    token->type = TT_id;
                    return OK;
                }
                break;

            /*_________OPERATORS________*/
            case S_lesserOrLesserEqual:
                if (c == '=') {
                    token->type = TT_lessEq;
                    return OK;
                } else {
                    ungetc(c, source);            // It's just <, undo last char read
                    token->type = TT_lesser;
                    return OK;
                }
                break;

             case S_greaterOrGreaterEqual:
                if (c == '=') {
                    token->type = TT_greatEq;
                    return OK;
                } else {
                    ungetc(c, source);            // It's just >, undo last char read
                    token->type = TT_greater;
                    return OK;
                }
                break;

            case S_assignOrEqual:
                if (c == '=') {
                    token->type = TT_equal;
                    return OK;
                } else {
                    ungetc(c, source);            // Just =, undo last char
                    token->type = TT_assign;
                    return OK;
                }
                break;

            case S_notOrNotEqual:
                if (c == '=') {
                    token->type = TT_notEq;
                    return OK;
                } else {
                    ungetc(c, source);            // Just =, undo last char
                    token->type = TT_not;
                    return OK;
                }
                break;

            case S_or:
                if (c == '|') {
                    token->type = TT_or;
                    return OK;
                } else {
                    return LEX_ERROR;
                }
                break;

            case S_and:
                if (c == '&') {
                    token->type = TT_and;
                    return OK;
                } else {
                    return LEX_ERROR;
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
                    return OK;
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
                    return OK;
                }
                break;

            case S_commentBlockStar:
                if (c == '*') {
                    ;
                } else if (c == '/') {
                    state = S_start;
                } else if (is_next_eof()) {
                    token->type = TT_eof;
                    return OK;
                } else {
                    state = S_commentBlock;
                }
                break;
        }
    }
    // Pokud funkce dojde az, sem musel byt nacteny charakter EOF
    token->type = TT_eof;
    return OK;
}
