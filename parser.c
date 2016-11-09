/*
 *  parser.c
 *  Jakub Semric
 *
 */

#include "token.h"
#include <stdio.h>
#include "string.h"
#include "globals.h"
#include "scanner.h"
#include "token_vector.h"
#include <stdbool.h>
#include <stdlib.h>
#include "debug.h"
#include "instruction.h"
#include "ial.h"

// symbol table size
#define RANGE 8
// unget token
static bool get_token_flag = false;
#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
T_symbol_table *symbol_tab;
T_token *token;
static int in_block = 0;
// pointer to actual class
static T_symbol *actual_class;

// each function represents a nonterminal symbol in LL(1) table
int prog();
int body();
int class();
int cbody();
int cbody2();
int func();
int fbody();
int par();
int par2();
int par3();
int st_list();
int stat();
int init();     // to be deleted latter
int ret();
int st_else();
int st_else2();
int type();     // to be deleted latter

// PROG -> BODY eof
int prog() {
    enter(__func__);

    return  leave(__func__, body());
}

// BODY -> CLASS BODY
// BODY -> e
int body() {

    enter(__func__);
    unsigned res = 0;
    // must read, because of recursion... (or not ?)
    if (get_token()) {
        return leave(__func__, LEX_ERROR);
    }

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        res = class();
        if (res)
            return res+leave(__func__, 0);

        return leave(__func__, body());
    }
    else if (token->type == TT_eof) {
        return leave(__func__, 0);
    }
    else
        return leave(__func__, SYNTAX_ERROR);
}

/* CLASS -> id lb CBODY rb
    symbol of type class defined
    actual class pointer changed
*/
int class() {
    enter(__func__);
    int res = 0;
    // going for id
    if (get_token())
        return leave(__func__, LEX_ERROR);

    // `id` expected
    if (token->type != TT_id)
        return leave(__func__, SYNTAX_ERROR);


    // going for '{'
    if (get_token())
         return leave(__func__, LEX_ERROR);

    if (token->type != TT_lCurlBracket)
        return leave(__func__, SYNTAX_ERROR);

    // cbody should return } in token
    res = cbody();

    return res+leave(__func__, 0);
}

/* CBODY -> static TYPE id CBODY2 CBODY
*/
int cbody() {
    enter(__func__);
    int res;
    // temporary variable for data type
    // static or '}'
    if (get_token())
        return leave(__func__, LEX_ERROR);

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        if (get_token())
            return leave(__func__, LEX_ERROR);

        // 'data type' expected
        if (token->type == TT_keyword) {
            switch (token->attr.keyword) {
                case TK_void:   // do stuff
                    break;
                case TK_int:
                    break;
                case TK_double:
                    break;
                case TK_String:
                    break;
                default:
                    return leave(__func__, SYNTAX_ERROR);
            }
        }
        else
            return leave(__func__, SYNTAX_ERROR);

        // 'id' expected
        if (get_token())
            return leave(__func__, LEX_ERROR);

        if (token->type != TT_id)
            return leave(__func__, SYNTAX_ERROR);

        res = cbody2(0);
        if (res)
            return leave(__func__, res);

        return cbody() + leave(__func__, 0);
    }
    else if (token->type == TT_rCurlBracket)
        return leave(__func__, 0);
    else
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, 0);
}

// TYPE -> void|int|double|string
// FIXME to be deleted
int type() {
    enter(__func__);
    if (token->type == TT_keyword) {
        switch (token->attr.keyword) {
            case TK_void:   // do stuff
            case TK_int:
            case TK_double:
            case TK_String:
                return leave(__func__, 0);
            default:
                return leave(__func__, SYNTAX_ERROR);
        }
        return leave(__func__, 0);
    }
    else
        return leave(__func__, SYNTAX_ERROR);
}


/* CBODY2 -> '=' ';' '('
*/
int cbody2() {
    enter(__func__);
    // '=' or ';' or '(' expected
    if (get_token())
        return leave(__func__, LEX_ERROR);

    // variable
    if (token->type == TT_assign || token->type == TT_semicolon) {
        token_vector tvect = token_vec_init();
        if (!tvect)
            return leave(__func__,INTERNAL_ERROR);

        while (token->type != TT_semicolon && token->type != TT_eof) {
            if (get_token()) {
                token_vec_delete(tvect);
                return leave(__func__, LEX_ERROR);
            }
            token_push_back(tvect, token);
        }
            //*********************************************************************
            // TODO call precedence analyser

            if (token->type == TT_semicolon)
                return leave(__func__, 0);
            return leave(__func__, SYNTAX_ERROR);
    }
    else if (token->type == TT_lBracket) {
        return func()+leave(__func__, 0);
    }
    else
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, SYNTAX_ERROR);
}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
int func() {
    enter(__func__);
    int res = 0;

    res = par();
    if (res)
        return leave(__func__, res);

    res = fbody();

    if (res)
        return leave(__func__, res);

    return leave(__func__, 0);
}

/*
    PAR -> ε | type id ...
*/
int par() {
    enter(__func__);
    // reading for ')' or type
    if (get_token()) {
        return leave(__func__, LEX_ERROR);
    }

    if (token->type == TT_rBracket)
        return leave(__func__, 0);

    token_vector tv = token_vec_init();
    if (!tv) {
        return leave(__func__, INTERNAL_ERROR);
    }

    // reading
    do {
        token_push_back(tv, token);
        if (get_token())
            return leave(__func__, LEX_ERROR);
    } while (token->type != TT_rBracket && token->type != TT_eof);


    token_vec_delete(tv);

    if (token->type == TT_eof)
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, 0);
/*

    if (token->type == TT_rBracket) {
        return leave(__func__, 0);
    }
    else if (token->type == TT_keyword) {
        // XXX attention do not reorder enum type T_keyword

    }
    else
        return leave(__func__, SYNTAX_ERROR);

    return par2(0)+leave(__func__, 0);*/
}

// PAR2 -> id PAR3
int par2() {
    enter(__func__);
    // reading for ')' or id
    if (get_token())
        return leave(__func__, LEX_ERROR);

    if (token->type == TT_rBracket)
        return leave(__func__, 0);
    else if (token->type == TT_id) {
        // TODO do stuff with id
        // add to par_list
    }
    else
        return leave(__func__, SYNTAX_ERROR);

    return par3(0)+leave(__func__, 0);


}

// PAR3 -> , TYPE id PAR3
int par3() {
    enter(__func__);
    // reading for ',' or ')'
    if (get_token())
        return leave(__func__, LEX_ERROR);

    if (token->type == TT_rBracket)
        return leave(__func__, 0);
    else if (token->type == TT_comma) {
        // reading for type
        if (get_token())
            return leave(__func__, LEX_ERROR);

        if (type())
            return leave(__func__, SYNTAX_ERROR);
        // id
        if (get_token())
            return leave(__func__, LEX_ERROR);
        if (token->type == TT_id) {
        // do stuff with id
        }
        else
            return leave(__func__, SYNTAX_ERROR);
    }
    else
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, par3(0));
}

// FBODY -> { ST_LIST }
// FBODY -> ;
int fbody() {
    enter(__func__);
    if (get_token())
        return leave(__func__, LEX_ERROR);

    if (token->type == TT_semicolon) {
        return leave(__func__, 0);
    }
    else if (token->type == TT_lCurlBracket) {
        return st_list()+leave(__func__, 0);
    }

    return leave(__func__, SYNTAX_ERROR);
}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
int st_list() {
    enter(__func__);
    // read only if '{' or ';'
    if (token->type == TT_lCurlBracket || token->type == TT_rCurlBracket ||
        token->type == TT_semicolon || token->type == TT_keyword) {
        // expecting whatever what is statement
        if (get_token())
            return leave(__func__, LEX_ERROR);
    }
    // keyword
    if (token->type == TT_keyword || token->type == TT_id) {

        int res = stat();
        if (res)
            return leave(__func__, res);

        return st_list()+leave(__func__, 0);
    }
    // empty statement
    else if (token->type == TT_semicolon) {
        return st_list()+leave(__func__, 0);
    }
    else if (token->type == TT_rCurlBracket) {
        return leave(__func__, 0);
    }
    else if (token->type == TT_lCurlBracket) {
        in_block++;
        int res = st_list();        // { ST-LIST } ST-LIST
        in_block--;
        if (res)
            return res+leave(__func__, 0);
        return st_list();
    }
    return leave(__func__, SYNTAX_ERROR);
}

// STAT -> many...
int stat() {
    enter(__func__);
    int bc = 0;
    int res = 0;
    // while|for|if|return|continue|break|types
    if (token->type == TT_keyword) {
        switch(token->attr.keyword) {
            case TK_int:
            case TK_double:
            case TK_String:
                {
                    // rule: STAT -> TYPE id INIT
                    if (in_block)
                        return leave(__func__, SYNTAX_ERROR);
                    res = type();
                    if (res)
                        return leave(__func__, 0) + res;

                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }
                    if (token->type != TT_id) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // TODO do stuff for id
                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }
                    if (token->type == TT_semicolon) {
                        return leave(__func__, 0);
                    }
                    else if (token->type == TT_assign) {
                        // creating token vector
                        token_vector tvect = token_vec_init();
                        // reading till ';' or 'eof' read
                        while (token->type != TT_semicolon && token->type != TT_eof) {
                            if (get_token()) {
                                token_vec_delete(tvect);
                                return leave(__func__, LEX_ERROR);
                            }
                            token_push_back(tvect, token);
                        }
                        // TODO call precedenceanalyser
                        token_vec_delete(tvect);
                        if (token->type == TT_semicolon)
                            return leave(__func__, 0);
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    else
                        return leave(__func__, SYNTAX_ERROR);
                }
            case TK_while:
                {
                    // ST -> while ( EXPR ) { ST_LIST }
                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }
                    if (token->type != TT_lBracket) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // reading whole expression
                    token_vector tvect = token_vec_init();
                    // reading till ';' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            token_vec_delete(tvect);
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                        token_push_back(tvect, token);
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }
                    // TODO call precedence analyser
                    token_vec_delete(tvect);

                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }

                    if (token->type != TT_lCurlBracket) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // begining new statement list
                    in_block++;
                    int res = st_list();
                    in_block--;
                    return res+leave(__func__, 0);
                }
            case TK_if:
                {
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }
                    if (token->type != TT_lBracket) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // reading whole expression
                    token_vector tvect = token_vec_init();
                    // reading till ';' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            token_vec_delete(tvect);
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                        token_push_back(tvect, token);
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }
                    // TODO call precedence analyser
                    token_vec_delete(tvect);

                    if (get_token()) {
                        return leave(__func__, LEX_ERROR);
                    }
                    // if (..) { ....
                    if (token->type != TT_lCurlBracket) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // beginning new statement list
                    int res;
                    in_block++;
                    res = st_list();
                    in_block--;
                    if (res)
                        return res+leave(__func__, 0);
                    // if next word is else do call st_else()
                    if (get_token())
                        return leave(__func__, LEX_ERROR);
                    if (token->type == TT_keyword && token->attr.keyword == TK_else)
                        return st_else()+leave(__func__, 0);
                    // no else read, must unget
                    unget_token();
                    return leave(__func__, 0);
                }
            case TK_return:
                // RET return EXPR;
                // RET return;
                // reading whole expression
                {
                    token_vector tvect = token_vec_init();
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            token_vec_delete(tvect);
                            return leave(__func__, LEX_ERROR);
                        }
                        token_push_back(tvect, token);
                    }

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }
                    // vector is not empty, expression handeler
                    if (tvect->last) {
                        // TODO call precedence analyser
                    }
                    token_vec_delete(tvect);

                    return leave(__func__, 0);
                }
            default:
                return leave(__func__, SYNTAX_ERROR);
        }
    }
    // id = exp
    else if (token->type == TT_id) {
        // inline rule
        if (get_token())
            return leave(__func__, LEX_ERROR);

        if (token->type == TT_dot) {
            if (get_token())
                return leave(__func__, LEX_ERROR);
            // id.id -> `(` or ` = ` or `;`
            if (token->type == TT_id) {
                if (get_token())
                    return leave(__func__, LEX_ERROR);
                // ;
                if (token->type == TT_semicolon)
                    return leave(__func__, 0);
                else if (token->type == TT_assign) {
                    // id.id = ........;
                    token_vector tvect = token_vec_init();
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            token_vec_delete(tvect);
                            return leave(__func__, LEX_ERROR);
                        }
                        token_push_back(tvect, token);
                    }
                    if (token->type == TT_eof) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    return leave(__func__, 0);
                }
                else if (token->type == TT_lBracket) {
                    // XXX func1()+func2();
                    // reading whole expression
                    token_vector tvect = token_vec_init();
                    // reading till ';' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            token_vec_delete(tvect);
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                        token_push_back(tvect, token);
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                        token_vec_delete(tvect);
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // TODO expression handling
                    token_vec_delete(tvect);
                    if (get_token())
                        return leave(__func__, LEX_ERROR);

                    if (token->type == TT_semicolon)
                        return leave(__func__, 0);
                }
                return leave(__func__, SYNTAX_ERROR);
            }
            return leave(__func__, SYNTAX_ERROR);
        }
        else if (token->type == TT_assign) {
            token_vector tvect = token_vec_init();
            // reading till ';' or 'eof' read
            while ( token->type != TT_semicolon && token->type != TT_eof) {
                if (get_token()) {
                    token_vec_delete(tvect);
                    return leave(__func__, LEX_ERROR);
                }
                token_push_back(tvect, token);
            }
            if (token->type == TT_eof) {
                return leave(__func__, SYNTAX_ERROR);
            }
            return leave(__func__, 0);
        }
        else if (token->type == TT_lBracket) {
            // id();
            // reading whole expression
            token_vector tvect = token_vec_init();
            // reading till ';' or 'eof' read
            bc = 0; // bracket counter
            do {
                if (get_token()) {
                    token_vec_delete(tvect);
                    return leave(__func__, LEX_ERROR);
                }
                if (token->type == TT_lBracket)
                    bc++;
                if (token->type == TT_rBracket)
                    bc--;
                token_push_back(tvect, token);
            } while ( bc != -1 && token->type != TT_eof);

            if (token->type == TT_eof) {
                token_vec_delete(tvect);
                return leave(__func__, SYNTAX_ERROR);
            }
            // TODO expression handling
            token_vec_delete(tvect);
            if (get_token())
                return leave(__func__, LEX_ERROR);

            if (token->type == TT_semicolon)
                return leave(__func__, 0);

            return leave(__func__, 0);
        }
    }
        return leave(__func__, SYNTAX_ERROR);
}

// INIT -> eq EXPR | ε
int init() {
    enter(__func__);
    if (token->type == TT_semicolon) {
        // do stuff with variable
        return leave(__func__, 0);
    }
    else if (token->type == TT_assign) {
        // call expresion analyser
        // do stuff with expresion, expresion analyser should get tokens until ;
        // then return to parser
    }
    return leave(__func__, 0);
}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
int st_else() {
    enter(__func__);

    if (get_token()) {
        return leave(__func__, LEX_ERROR);
    }
    // `else {`
    if (token->type == TT_lCurlBracket) {
        in_block++;
        int res = st_list()+leave(__func__, 0);
        in_block--;
        return res;
    }
    // `else if`
    else if (token->type == TT_keyword && token->attr.keyword == TK_if) {
        return st_else2()+leave(__func__, 0);
    }

    return leave(__func__, SYNTAX_ERROR);
}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
int st_else2() {
    enter(__func__);

    if (get_token()) {
        return leave(__func__, LEX_ERROR);
    }
    // only '('
    if (token->type == TT_lBracket) {
        // reading whole expression
        token_vector tvect = token_vec_init();
        // reading till ')' or 'eof' read
        int bc = 0; // bracket counter
        do {
            if (get_token()) {
                token_vec_delete(tvect);
                return leave(__func__, LEX_ERROR);
            }
            if (token->type == TT_lBracket)
                bc++;
            if (token->type == TT_rBracket)
                bc--;
            token_push_back(tvect, token);
        } while ( bc != -1 && token->type != TT_eof);

        if (token->type == TT_eof) {
            return leave(__func__, SYNTAX_ERROR);
        }
        // TODO call precedence analyser
        token_vec_delete(tvect);

        if (get_token()) {
            return leave(__func__, LEX_ERROR);
        }
        // begining of statement list
        if (token->type != TT_lCurlBracket) {
            return leave(__func__, SYNTAX_ERROR);
        }
        int res;
        in_block++;
        res = st_list();
        in_block--;
        if (res)
            return leave(__func__, res);
        // if next word is else do call st_else()
        if (get_token())
            return leave(__func__, LEX_ERROR);
        if (token->type == TT_keyword && token->attr.keyword == TK_else)
            return leave(__func__, st_else());
        // no else, token must be returned
        unget_token();
        return leave(__func__, 0);
    }
    return leave(__func__, SYNTAX_ERROR);
}

int parse() {

    if (!(token = token_new()) ) {
        fprintf(stderr, "Internal error\n");
        return INTERNAL_ERROR;
    }

    symbol_tab = table_init(RANGE);

/*
    while (token->type != TT_eof)
        get_token();
*/
    int res;

    res =  prog();
    if (res == LEX_ERROR)
        fprintf(stderr, "Lexical error\n");
    else if (res == SYNTAX_ERROR)
        fprintf(stderr, "Syntax error\n");
    else if (res == DEFINITION_ERROR)
        fprintf(stderr, "Definition error\n");
    else if (res == INTERNAL_ERROR)
        fprintf(stderr, "Internal error\n");
    else if (res == TYPE_ERROR)
        fprintf(stderr, "Type error\n");
    else if (res == SEMANTIC_ERROR)
        fprintf(stderr, "Semantic error\n");
    else
        fprintf(stderr, "Success\n");

    table_remove(&symbol_tab);
    token_free(&token);
    return res;

}

