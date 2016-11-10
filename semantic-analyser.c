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

// unget token
static bool get_token_flag = false;
#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
T_symbol_table *symbol_tab;
T_token *token;
// pointer to actual class
static T_symbol *actual_class;
static T_symbol *actual_func;

// each function represents a nonterminal symbol in LL(1) table
static int prog();
static int body();
static int class();
static int cbody();
static int cbody2(T_symbol_table *local_tab);
static int func(T_symbol_table *local_tab);
static int fbody(T_symbol_table *local_tab);
static int par();
static int st_list(T_symbol_table *local_tab);
static int stat(T_symbol_table *local_tab);
static int st_else();
static int st_else2();

// PROG -> BODY eof
static int prog()
{{{
    enter(__func__);

    return  leave(__func__, body());
}}}

// BODY -> CLASS BODY
// BODY -> e
static int body()
{{{

    enter(__func__);
    unsigned res = 0;
    get_token();

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        res = class();
        if (res)
            return leave(__func__, res);

        return leave(__func__, body());
    }
    else if (token->type == TT_eof) {
        return leave(__func__, 0);
    }

    return leave(__func__, SYNTAX_ERROR);
}}}

/* CLASS -> id lb CBODY rb
*/
static int class()
{{{
    enter(__func__);
    // going for id
    get_token();

    // already tested in by first throughpass
    actual_class = table_find(symbol_tab, token->attr.str->buf, NULL);

    // going for '{'
    get_token();

    return leave(__func__, cbody());
}}}

/*
   CBODY -> static TYPE id CBODY2 CBODY
*/
static int cbody()
{{{
    enter(__func__);
    int res;
    // static or '}'
    get_token();

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        get_token();

        // 'id' expected
        get_token();


        // in case that symbol is function local symbol table is set
        T_symbol *s = table_find(symbol_tab,token->attr.str->buf, actual_class);
        if (s->symbol_type == is_func) {
            // updating actual function
            actual_func = s;
            // local table
            T_symbol_table *local_tab = s->attr.func->local_table;
            res = cbody2(local_tab);
        } else {
            res = cbody2(NULL);
        }

        if (res)
            return leave(__func__, res);

        return leave(__func__, cbody());
    }
    else
        return leave(__func__, 0);
}}}

/*
   CBODY2 -> '=' ';' '('
*/
static int cbody2(T_symbol_table *local_tab)
{{{
    enter(__func__);
    // '=' or ';' or '(' expected
    get_token();

    // variable
    if (token->type == TT_assign || token->type == TT_semicolon) {

        // just reading everything till `;`
        while (token->type != TT_semicolon) {
            // TODO fill token vector, handle expression
            get_token();
        }

        return leave(__func__, 0);
    }
    else {
        return leave(__func__, func(local_tab));
    }
}}}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
static int func(T_symbol_table *local_tab)
{{{
    enter(__func__);
    // just read parameters
    par();
    // process body
    int res = fbody(local_tab);
    print_function(actual_func);
    return leave(__func__, res);
}}}

/*
   just read till ')'
*/
static int par()
{{{
    enter(__func__);
    while (token->type != TT_rBracket)
        get_token();
    return leave(__func__, 0);
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static int fbody(T_symbol_table *local_tab)
{{{
    enter(__func__);
    get_token();

    if (token->type == TT_semicolon) {
        return leave(__func__, 0);
    }
    else {
        return leave(__func__, st_list(local_tab));
    }

}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static int st_list(T_symbol_table *local_tab) {{{
    enter(__func__);
    // read only if '{' or ';'
    if (token->type == TT_lCurlBracket || token->type == TT_rCurlBracket ||
        token->type == TT_semicolon || token->type == TT_keyword) {
        // expecting whatever what is statement
        get_token();
    }
    // keyword
    if (token->type == TT_keyword || token->type == TT_id) {

        int res = stat(local_tab);
        if (res)
            return leave(__func__, res);

        return st_list(local_tab)+leave(__func__, 0);
    }
    // empty statement
    else if (token->type == TT_semicolon) {
        return st_list(local_tab)+leave(__func__, 0);
    }
    else if (token->type == TT_rCurlBracket) {
        return leave(__func__, 0);
    }
    else if (token->type == TT_lCurlBracket) {
        int res = st_list(local_tab);        // { ST-LIST } ST-LIST
        if (res)
            return res+leave(__func__, 0);
        return st_list(local_tab);
    }
    return leave(__func__, SYNTAX_ERROR);
}}}

// STAT -> many...
static int stat(T_symbol_table *local_tab) {
    enter(__func__);
    int bc = 0;
    // while|for|if|return|continue|break|types
    if (token->type == TT_keyword) {
        switch(token->attr.keyword) {
            case TK_int:
            case TK_double:
            case TK_String:
                {{{
                    // rule: STAT -> TYPE id ;| = EXPR ;

                    // allocate space for new symbol
                    T_symbol *symbol = calloc(1, sizeof(T_symbol));
                    if (!symbol) {
                        return leave(__func__, INTERNAL_ERROR);
                    }

                    // allocate space for variable
                    symbol->attr.var = calloc(1, sizeof(T_var_symbol));
                    if (!symbol->attr.var) {
                         return leave(__func__, INTERNAL_ERROR);
                    }

                    // initialize string variable
                    if (symbol->data_type == is_str) {
                        if ( !(symbol->attr.var->value.str = str_init()) )
                            return leave(__func__, INTERNAL_ERROR);
                    }

                    symbol->data_type = token->attr.keyword; // set data type
                    symbol->symbol_type = is_var;   // set symbol type

                    // id in token
                    get_token();
                    if (table_find(local_tab, token->attr.str->buf, NULL)) {
                        // redefinition
                        return leave(__func__, DEFINITION_ERROR);
                    }
                    symbol->id = token->attr.str->buf;  // set var name
                    token->attr.str->buf = NULL;        // discredit free call
                    table_insert(local_tab, symbol);    // insert to table

                    // ; or =
                    get_token();

                    if (token->type == TT_semicolon) {
                        return leave(__func__, 0);
                    }
                    else {
                        // assign '='
                        while (token->type != TT_semicolon) {
                            get_token();
                            // TODO expression + token vector
                        }
                        return leave(__func__, 0);
                    }
                    /*
                    free_var:
                    free(symbol->attr)
                    free_symbol:
                    free(symbol);
                    return leave(__func__, rc);*/
                }}}
            case TK_while:
                {{{
                    // ST -> while ( EXPR ) { ST_LIST }

                    // '('
                    get_token();

                    // reading `)` or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        get_token();
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                        // TODO expression
                    } while ( bc != -1);
                    // {
                    get_token();

                    // beginning new statement list
                    return leave(__func__, st_list(local_tab));
                }}}
            case TK_if:
                {{{
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    get_token();
                    // reading till ')'
                    bc = 0; // bracket counter
                    do {
                        get_token();
                        // TODO expression
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 );

                    get_token();
                    // if (..) { ....
                    // beginning new statement list
                    int res;
                    res = st_list(local_tab);
                    if (res)
                        return leave(__func__, res);
                    // if next word is else do call st_else()
                    get_token();

                    if (token->type == TT_keyword && token->attr.keyword == TK_else)
                        return leave(__func__, st_else());
                    // no else read, must unget
                    unget_token();
                    return leave(__func__, 0);
                }}}
            case TK_return:
                {{{
                    // RET return EXPR;
                    // RET return;
                    // reading whole expression
                    {
                        // reading till ';' or 'eof' read
                        while ( token->type != TT_semicolon) {
                            // TODO expression
                            get_token();
                        }

                        return leave(__func__, 0);
                    }
                }}}
            default:
                return leave(__func__, SYNTAX_ERROR);
        }
    }
    // id = exp
    else {
        // inline rule
        // TODO check id
        get_token();

        if (token->type == TT_dot)
        {{{
            get_token();
            // TODO check class id
            // id.id -> `(` or ` = ` or `;`
            // TODO check id
            get_token();
            // ; - dead code
            if (token->type == TT_semicolon) {
                return leave(__func__, 0);
            }
            else if (token->type == TT_assign) {
                // id.id = ........;
                // reading till ';' or 'eof' read
                while ( token->type != TT_semicolon) {
                    get_token();
                    // TODO expression
                }
                return leave(__func__, 0);
            }
            else {
                // id.id()
                // XXX func1()+func2();
                // reading whole expression
                // reading till ';' or 'eof' read
                bc = 0; // bracket counter
                do {
                    get_token();
                    if (token->type == TT_lBracket)
                        bc++;
                    if (token->type == TT_rBracket)
                        bc--;
                    // TODO expression
                } while ( bc != -1);

                // TODO expression handling
                // ;
                get_token();

                return leave(__func__, 0);
            }
        }}}
        else if (token->type == TT_assign)
        {{{
            // reading till ';' or 'eof' read
            while ( token->type != TT_semicolon) {
                get_token();
                // TODO expression
            }
            return leave(__func__, 0);
        }}}
        else
        {{{
            // id();
            // reading whole expression
            // reading till ';' or 'eof' read
            bc = 0; // bracket counter
            do {
                get_token();
                // TODO expression
                if (token->type == TT_lBracket)
                    bc++;
                if (token->type == TT_rBracket)
                    bc--;
            } while ( bc != -1);

            // ;
            get_token();

            return leave(__func__, 0);
        }}}
    }
}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static int st_else()
{{{
    enter(__func__);

    get_token();
    // `else {`
    if (token->type == TT_lCurlBracket) {
        int res = st_list(NULL)+leave(__func__, 0);
        return res;
    }
    // `else if`
    else {
        return st_else2()+leave(__func__, 0);
    }

}}}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
static int st_else2()
{{{
    enter(__func__);

    get_token();
    // only '('
    // reading till ')' or 'eof' read
    int bc = 0; // bracket counter
    do {
        // TODO expression
        get_token();
        if (token->type == TT_lBracket)
            bc++;
        if (token->type == TT_rBracket)
            bc--;
    } while ( bc != -1);

    // {
    get_token();
    // beginning of statement list
    int res;
    res = st_list(NULL);
    if (res)
        return leave(__func__, res);
    // if next word is else do call st_else()
    get_token();

    if (token->type == TT_keyword && token->attr.keyword == TK_else)
        return leave(__func__, st_else());
    // no else, token must be returned
    unget_token();
    return leave(__func__, 0);
}}}

int second_throughpass() {
    return prog();
}

