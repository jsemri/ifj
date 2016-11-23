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

/*
 * token.h
 *
 * File contains definition of structure of a token and member functions.
 *
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

// keywords
typedef enum {
    TK_void = 0,
    TK_int = 1,
    TK_double = 2,
    TK_String = 3,
    TK_boolean,
    TK_break,
    TK_class,
    TK_continue,
    TK_do,
    TK_else,
    TK_false,
    TK_for,
    TK_if,
    TK_return,
    TK_static,
    TK_true,
    TK_while,
} T_keyword;


// token types
typedef enum {
    TT_empty,
    TT_int,
    TT_double,
    TT_string,
    TT_plus,            // +
    TT_minus,           // -
    TT_mul,             // *
    TT_div,             // /
    TT_greater,         // >
    TT_lesser,          // <
    TT_greatEq,         // >=
    TT_lessEq,          // <=
    TT_equal,           // ==
    TT_notEq,           // !=
    TT_semicolon,       // ;
    TT_assign,          // =
    TT_comma,           // ,
    TT_dot,             // .    - will be deleted soon
    TT_lBracket,        // (
    TT_rBracket,        // )
    TT_lCurlBracket,    // {
    TT_rCurlBracket,    // }
    TT_or,              // ||
    TT_and,             // &&
    TT_not,             // !
    TT_id,              // identifier
    TT_keyword,         // keyword
    TT_fullid,          // id.id
    TT_eof              // end of file
} T_tokenType;

typedef struct {
    double d;           // values double or integer
    int n;
    T_keyword keyword;  // keyword
    char *str;          // pointer to T_string
    bool b;             // boolean
} T_value;

// structure of token
typedef struct {
    T_tokenType type;
    T_value attr;
} T_token;

// allocates and initializes new token
T_token* token_new();
// removes string from token
void token_clear(T_token *t);
// removes token
void token_free(T_token **t);
#endif
