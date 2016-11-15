/*
 *  parser.c
 *  Jakub Semric
 *
 */

#include "token.h"
#include <stdio.h>
#include "string.h"
#include <string.h>
#include "globals.h"
#include "scanner.h"
#include "token_vector.h"
#include <stdbool.h>
#include <stdlib.h>
#include "debug.h"
#include "instruction.h"
#include "ial.h"
#include "semantic-analyser.h"
#include "symbol.h"


#ifdef DEBUG
#define return return leave(__func__, 0) +
#endif

// unget token
static bool get_token_flag = false;
#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
T_symbol_table *symbol_tab;
T_token *token;
FILE *source;
static int in_block = 0;
// pointer to actual class
static T_symbol *actual_class;

// each function represents a nonterminal symbol in LL(1) table
static int prog();
static int body();
static int class();
static int cbody();
static int cbody2(T_symbol *symbol, T_data_type dtype);
static int func(T_symbol *symbol, T_data_type dtype);
static int fbody();
static int par(T_symbol *symbol);
static int st_list();
static int stat();
static int st_else();
static int st_else2();

// PROG -> BODY eof
static int prog()
{{{
    enter(__func__);

    return   body();
}}}

// BODY -> CLASS BODY
// BODY -> e
static int body()
{{{

    enter(__func__);
    unsigned res = 0;
    // must read, because of recursion... (or not ?)
    if (get_token()) {
        return LEX_ERROR;
    }

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        res = class();
        if (res)
            return res;

        return body();
    }
    else if (token->type == TT_eof) {
        return 0;
    }
    else
        return SYNTAX_ERROR;
}}}

/* CLASS -> id lb CBODY rb
   checking syntax
   inserting symbol `class`
   changing actual class poiter
*/
static int class()
{{{
    enter(__func__);
    // return value
    int res = 0;
    // going for id
    if (get_token())
        return LEX_ERROR;

    // `id` expected
    if (token->type != TT_id)
        return SYNTAX_ERROR;
    T_symbol *symbol;
    // finding a symbol
    if ((symbol = table_find(symbol_tab, token->attr.str->buf, NULL))) {
        // class with same name found - definition error
        return DEFINITION_ERROR;
    }

    symbol = create_symbol(token->attr.str->buf, is_class);
    token->attr.str->buf = NULL;        // no deep copy of char*
    table_insert(symbol_tab, symbol);

    actual_class = symbol;

    // going for '{'
    if (get_token())
         return LEX_ERROR;

    if (token->type != TT_lCurlBracket)
        return SYNTAX_ERROR;

    // cbody should return } in token
    res = cbody();

    return res;
}}}

/*
   CBODY -> static TYPE id CBODY2 CBODY
   symbol data type, member class, id defined
*/
static int cbody()
{{{
    enter(__func__);
    int res;
    unsigned dtype;     // data type
    // static or '}'
    if (get_token())
        return LEX_ERROR;

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        if (get_token())
            return LEX_ERROR;

        // 'data type' expected
        if (token->type == TT_keyword && token->attr.keyword < TK_boolean) {
            dtype = token->attr.keyword;
        }
        else {
            return SYNTAX_ERROR;
        }

        // 'id' expected
        if (get_token())
            return LEX_ERROR;

        if (token->type != TT_id)
            return SYNTAX_ERROR;

        // finding a symbol
        T_symbol *symbol;
        symbol = table_find(symbol_tab, token->attr.str->buf, actual_class);

        if ( symbol ){
            // id in class with same name found - definition error
            return DEFINITION_ERROR;
        }

        // creating a symbol
        symbol = create_symbol(token->attr.str->buf, 0);
        token->attr.str->buf = 0;
        symbol->member_class = actual_class;
        table_insert(symbol_tab, symbol);

        res = cbody2(symbol, dtype);
        if (res)
            return res;

        return cbody();
    }
    else if (token->type == TT_rCurlBracket)
        return 0;
    else
        return SYNTAX_ERROR;
}}}

/*
   CBODY2 -> '=' ';' '('
    setting variable or function
*/
static int cbody2(T_symbol *symbol, T_data_type dtype)
{{{
    enter(__func__);
    // '=' or ';' or '(' expected
    if (get_token())
        return LEX_ERROR;

    // variable
    if (token->type == TT_assign || token->type == TT_semicolon) {

        if (dtype == is_void) {
            return TYPE_ERROR;
        }
        if (!(symbol->attr.var = create_var(dtype))) {
            return INTERNAL_ERROR;
        }
        // setting symbol as variable
        if (!(symbol->attr.var = create_var(dtype))) {
            return INTERNAL_ERROR;
        }
        symbol->symbol_type = is_var;

        // initialization
        // just reading everything till `;`
        while (token->type != TT_semicolon && token->type != TT_eof) {
            if (get_token()) {
                return LEX_ERROR;
            }
        }

        if (token->type == TT_semicolon)
            return 0;
        return SYNTAX_ERROR;
    }
    else if (token->type == TT_lBracket) {
        // function
        return func(symbol, dtype);
    }
    else
        return SYNTAX_ERROR;
}}}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
static int func(T_symbol *symbol, T_data_type dtype)
{{{
    enter(__func__);
    int res = 0;

    // creating a function
    if (!(symbol->attr.func = create_func(dtype))) {
        return INTERNAL_ERROR;
    }
    symbol->symbol_type = is_func;

    res = par(symbol);
    if (res)
        return res;

    res = fbody(symbol);

    // printing local table - just for debug
    print_table(symbol->attr.func->local_table);

    if (res)
        return res;

    return 0;
}}}

/*
    PAR -> ε | type id ...
*/
static int par(T_symbol *symbol)
{{{
    enter(__func__);

    int rc = SYNTAX_ERROR;
    // reading for ')' or type
    if (get_token()) {
        return LEX_ERROR;
    }

    if (token->type == TT_rBracket) {
        // no parameters
        symbol->attr.func->par_count = 0;
        symbol->attr.func->arguments = NULL;
        return 0;
    }

    token_vector tv = token_vec_init();

    do {
        token_push_back(tv, token);
        if (get_token()) {
            rc = LEX_ERROR;
            goto free_tvect;
        }
    } while (token->type != TT_rBracket && token->type != TT_eof);

    // pushing ')'
    token_push_back(tv, token);

    if (token->type == TT_eof) {
        goto free_tvect;
    }

    T_func_symbol *fptr = symbol->attr.func;
    // number of read tokens
    if ((tv->last)%3) {
        goto free_tvect;
    }
    fptr->par_count = (tv->last)/3;

    // allocating pointer to arguments
    void **arguments = calloc(1,sizeof(void*)*fptr->par_count);
    if (!arguments) {
        rc = INTERNAL_ERROR;
        goto free_tvect;
    }

    unsigned i = 0; // parameter count
    // resolving parameters of function
    T_token *tptr = tv->arr;
    while (tptr->type != TT_rBracket) {
        // data type
        unsigned dtype;
        // data type expected
        if (tptr->type == TT_keyword && tptr->attr.keyword < TK_boolean) {
            dtype = tptr->attr.keyword;
            // XXX maybe other error code ???
            if (dtype == TK_void)
                return TYPE_ERROR;
        }
        else {
            goto free_args;
        }
        // identifier expected
        tptr++;
        if (tptr->type != TT_id) {
            goto free_args;
        }
        // argument names must differ
        if (table_find(fptr->local_table, tptr->attr.str->buf, NULL)) {
            rc = DEFINITION_ERROR;
            goto free_args;
        }

        // create and insert symbol
        T_symbol *arg_var = create_symbol(tptr->attr.str->buf, is_var);
        tptr->attr.str->buf = NULL;         // discredit token buffer

        // creating variable
        arg_var->attr.var = create_var(dtype);

        arguments[i++] = arg_var;

        // inserting variable
        table_insert(fptr->local_table, arg_var);

        // skipping ','
        tptr++;
        if (tptr->type == TT_comma) {
            tptr++;
        }
        else if (tptr->type != TT_rBracket) {
            goto free_args;
        }
    }

    fptr->arguments = arguments;
    token_vec_delete(tv);
    return 0;

    // errors
    free_args:
    free(arguments);
    free_tvect:
    token_vec_delete(tv);
    return rc;
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static int fbody()
{{{
    enter(__func__);
    if (get_token())
        return LEX_ERROR;

    if (token->type == TT_semicolon) {
        return 0;
    }
    else if (token->type == TT_lCurlBracket) {
        return st_list();
    }

    return SYNTAX_ERROR;
}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static int st_list()
{{{
    enter(__func__);
    // read only if '{' or ';'
    if (token->type == TT_lCurlBracket || token->type == TT_rCurlBracket ||
        token->type == TT_semicolon || token->type == TT_keyword) {
        // expecting whatever what is statement
        if (get_token())
            return LEX_ERROR;
    }
    // keyword
    if (token->type == TT_keyword || token->type == TT_id) {

        int res = stat();
        if (res)
            return res;

        return st_list();
    }
    else if (token->type == TT_rCurlBracket) {
        return 0;
    }
    else if (token->type == TT_lCurlBracket) {
        in_block++;
        int res = st_list();        // { ST-LIST } ST-LIST
        in_block--;
        if (res)
            return res;
        return st_list();
    }
    return SYNTAX_ERROR;
}}}

// STAT -> many...
static int stat()
{{{
    enter(__func__);
    int bc = 0;
    // while|for|if|return|continue|break|types
    if (token->type == TT_keyword) {
        switch(token->attr.keyword) {
            case TK_int:
            case TK_double:
            case TK_String:
                {
                    // rule: STAT -> TYPE id ;| = EXPR ;
                    // local variable cannot be declared in block {...}
                    if (in_block)
                        return SYNTAX_ERROR;

                    if (get_token()) {
                        return LEX_ERROR;
                    }
                    if (token->type != TT_id) {
                        return SYNTAX_ERROR;
                    }
                    if (get_token()) {
                        return LEX_ERROR;
                    }
                    if (token->type == TT_semicolon) {
                        return 0;
                    }
                    else if (token->type == TT_assign) {
                        while (token->type != TT_semicolon && token->type != TT_eof) {
                            if (get_token()) {
                                return LEX_ERROR;
                            }
                        }
                        if (token->type == TT_semicolon)
                            return 0;
                        return SYNTAX_ERROR;
                    }
                    else
                        return SYNTAX_ERROR;
                }
           case TK_while:
           case TK_if:
                {
                    // while or if
                    T_keyword keyword = token->attr.keyword;
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    if (get_token()) {
                        return LEX_ERROR;
                    }
                    if (token->type != TT_lBracket) {
                        return SYNTAX_ERROR;
                    }
                    // reading till ')' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            return LEX_ERROR;
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         return SYNTAX_ERROR;
                    }

                    if (get_token()) {
                        return LEX_ERROR;
                    }
                    // if (..) { ....
                    if (token->type != TT_lCurlBracket) {
                        return SYNTAX_ERROR;
                    }
                    // beginning new statement list
                    int res;
                    in_block++;
                    res = st_list();
                    in_block--;
                    if (res)
                        return res;

                    // end while
                    if (keyword == TK_while) {
                        return 0;
                    }

                    // if next word is else do call st_else()
                    if (get_token())
                        return LEX_ERROR;
                    if (token->type == TT_keyword && token->attr.keyword == TK_else)
                        return st_else();
                    // no else read, must unget
                    unget_token();
                    return 0;
                }
            case TK_return:
                // RET return EXPR;
                // RET return;
                // reading whole expression
                {
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            return LEX_ERROR;
                        }
                    }

                    if (token->type == TT_eof) {
                         return SYNTAX_ERROR;
                    }

                    return 0;
                }
            default:
                return SYNTAX_ERROR;
        }
    }
    // id = exp
    else if (token->type == TT_id) {

        // inline rule
        if (get_token())
            return LEX_ERROR;

        if (token->type == TT_dot) {
            if (get_token())
                return LEX_ERROR;
            // id.id -> `(` or ` = ` or `;`
            if (token->type == TT_id) {
                if (get_token()) {
                    return LEX_ERROR;
                }
                if (token->type == TT_assign) {
                    // id.id = ........;
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            return LEX_ERROR;
                        }
                    }
                    if (token->type == TT_eof) {
                        return SYNTAX_ERROR;
                    }
                    return 0;
                }
                else if (token->type == TT_lBracket) {
                    // reading whole expression
                    // reading till ';' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            return LEX_ERROR;
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                        return SYNTAX_ERROR;
                    }
                    if (get_token())
                        return LEX_ERROR;

                    if (token->type == TT_semicolon)
                        return 0;
                }
                return SYNTAX_ERROR;
            }
            return SYNTAX_ERROR;
        }
        else if (token->type == TT_assign) {
            // reading till ';' or 'eof' read
            while ( token->type != TT_semicolon && token->type != TT_eof) {
                if (get_token()) {
                    return LEX_ERROR;
                }
            }
            if (token->type == TT_eof) {
                return SYNTAX_ERROR;
            }
            return 0;
        }
        else if (token->type == TT_lBracket) {
            // id();
            // reading whole expression
            // reading till ';' or 'eof' read
            bc = 0; // bracket counter
            do {
                if (get_token()) {
                    return LEX_ERROR;
                }
                if (token->type == TT_lBracket)
                    bc++;
                if (token->type == TT_rBracket)
                    bc--;
            } while ( bc != -1 && token->type != TT_eof);

            if (token->type == TT_eof) {
                return SYNTAX_ERROR;
            }

            if (get_token())
                return LEX_ERROR;

            if (token->type == TT_semicolon)
                return 0;

            return SYNTAX_ERROR;
        }
    }
        return SYNTAX_ERROR;
}}}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static int st_else()
{{{
    enter(__func__);

    if (get_token()) {
        return LEX_ERROR;
    }
    // `else {`
    if (token->type == TT_lCurlBracket) {
        in_block++;
        int res = st_list()+0;
        in_block--;
        return res;
    }
    // `else if`
    else if (token->type == TT_keyword && token->attr.keyword == TK_if) {
        return st_else2()+0;
    }

    return SYNTAX_ERROR;
}}}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
static int st_else2()
{{{
    enter(__func__);

    if (get_token()) {
        return LEX_ERROR;
    }
    // only '('
    if (token->type == TT_lBracket) {
        // reading till ')' or 'eof' read
        int bc = 0; // bracket counter
        do {
            if (get_token()) {
                return LEX_ERROR;
            }
            if (token->type == TT_lBracket)
                bc++;
            if (token->type == TT_rBracket)
                bc--;
        } while ( bc != -1 && token->type != TT_eof);

        if (token->type == TT_eof) {
            return SYNTAX_ERROR;
        }

        if (get_token()) {
            return LEX_ERROR;
        }
        // begining of statement list
        if (token->type != TT_lCurlBracket) {
            return SYNTAX_ERROR;
        }
        int res;
        in_block++;
        res = st_list();
        in_block--;
        if (res)
            return res;
        // if next word is else do call st_else()
        if (get_token())
            return LEX_ERROR;
        if (token->type == TT_keyword && token->attr.keyword == TK_else)
            return st_else();
        // no else, token must be returned
        unget_token();
        return 0;
    }
    return SYNTAX_ERROR;
}}}

int parse()
{{{

    if (!(token = token_new()) ) {
        return INTERNAL_ERROR;
    }

    // create global symbol table
    if (!(symbol_tab = table_init(RANGE))) {
        token_free(&token);
        return INTERNAL_ERROR;
    }

    // return value
    int res = INTERNAL_ERROR;

    // insert ifj16
    fill_ifj16();

    /*
       lexical + syntax analysis
       filling symbol table + checking redefinitions
    */
    res = prog();
    if (res)
        goto errors;

    // checking Main class and run() function
    T_symbol *Mainclass = table_find(symbol_tab, "Main", NULL);
    if (!Mainclass) {
        res = SEMANTIC_ERROR;
        goto errors;
    }
    T_symbol *run_func = table_find(symbol_tab, "run", Mainclass);
    if (!run_func || run_func->symbol_type != is_func) {
        res = SEMANTIC_ERROR;
        goto errors;
    }

    // just for debug
    print_table(symbol_tab);

    if (fseek(source, 0, SEEK_SET)) {
        res = INTERNAL_ERROR;
        goto errors;
    } else {
        #ifdef DEBUG
        puts("_____________________________________________\n\n\n");
        #endif
        res = second_throughpass();
    }


    errors:
    table_remove(&symbol_tab);
    token_free(&token);
    return res;
}}}


