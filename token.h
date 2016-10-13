/*
 * token.h
 *
 * File contains just a structure of a token. Rest will be done later.
 *
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "string.h"

// keywords
typedef enum {
    TK_boolean,
	TK_break,
	TK_class,
	TK_continue,
	TK_do,
	TK_double,
	TK_else,
	TK_false,
	TK_for,
	TK_if,
	TK_int,
	TK_return,
	TK_String,
	TK_static,
	TK_true,
	TK_void,
	TK_while
} T_keyword;


// token types
typedef enum {
    TT_plus,            // +
	TT_minus,           // -
	TT_mul,             // *
	TT_div,             // /
	TT_greater,         // >
	TT_lesser,          // <
	TT_gratEq,          // >=
	TT_lessEq,          // <=
	TT_equal,           // ==
	TT_notEq,           // !=
	TT_semicolon,       // ;
	TT_assign,          // =
	TT_comma,           // ,
	TT_dot,             // .
	TT_lBracket,        // (
	TT_rBracket,        // )
	TT_lCurlBracket,    // {
	TT_rCurlBracket,    // }
	TT_or,              // ||
	TT_and,             // &&
	TT_not,             // !
	TT_id,              // identifier
	TT_keyword,         // keyword
	TT_number,          // int constant
	TT_string,          // string literal
	TT_double,          // double constant
	TT_eof              // end of file
} T_tokenType;

// structure of token
typedef struct {
	T_tokenType type;
    union {
        double d;       // values
        int n;
        T_keyword keyword;  // keyword
        String str;     // "string.h"
    };
} Token;

// TODO
// token functions

#endif
