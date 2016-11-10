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
#include "semantic-analyser.h"

// symbol table size
#define RANGE 8
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
static int cbody2(T_symbol *symbol);
static int func(T_symbol *symbol);
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

    return  leave(__func__, body());
}}}

// BODY -> CLASS BODY
// BODY -> e
static int body()
{{{

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
        return leave(__func__, LEX_ERROR);

    // `id` expected
    if (token->type != TT_id)
        return leave(__func__, SYNTAX_ERROR);
    T_symbol *symbol;
    // finding a symbol
    if ((symbol = table_find(symbol_tab, token->attr.str->buf, NULL))) {
        // class with same name found - definition error
        return leave(__func__, DEFINITION_ERROR);
    }


    // creating a symbol
    if ( !(symbol = calloc(1, sizeof(T_symbol)) ) ) {
        return leave(__func__, INTERNAL_ERROR);
    }
    symbol->symbol_type = is_class;
    symbol->id = token->attr.str->buf;
    token->attr.str->buf = 0;
    table_insert(symbol_tab, symbol);

    actual_class = symbol;

    // going for '{'
    if (get_token())
         return leave(__func__, LEX_ERROR);

    if (token->type != TT_lCurlBracket)
        return leave(__func__, SYNTAX_ERROR);

    // cbody should return } in token
    res = cbody();

    return res+leave(__func__, 0);
}}}

/*
   CBODY -> static TYPE id CBODY2 CBODY
   symbol data type, member class, id defined
*/
static int cbody()
{{{
    enter(__func__);
    int res;
    unsigned data_type;
    // static or '}'
    if (get_token())
        return leave(__func__, LEX_ERROR);

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        if (get_token())
            return leave(__func__, LEX_ERROR);

        // 'data type' expected
        if (token->type == TT_keyword && token->attr.keyword < TK_boolean) {
            data_type = token->attr.keyword;
        }
        else
            return leave(__func__, SYNTAX_ERROR);

        // 'id' expected
        if (get_token())
            return leave(__func__, LEX_ERROR);

        if (token->type != TT_id)
            return leave(__func__, SYNTAX_ERROR);

        // finding a symbol
        T_symbol *symbol;
        symbol = table_find(symbol_tab, token->attr.str->buf, actual_class);

        if ( symbol ){
            // id in class with same name found - definition error
            return leave(__func__, DEFINITION_ERROR);
        }

        // creating a symbol
        if ( !(symbol = calloc(1, sizeof(T_symbol)) ) ) {
            return leave(__func__, INTERNAL_ERROR);
        }
        symbol->id = token->attr.str->buf;
        token->attr.str->buf = 0;
        symbol->data_type = data_type;
        symbol->member_class = actual_class;
        table_insert(symbol_tab, symbol);

        res = cbody2(symbol);
        if (res)
            return leave(__func__, res);

        return cbody() + leave(__func__, 0);
    }
    else if (token->type == TT_rCurlBracket)
        return leave(__func__, 0);
    else
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, 0);
}}}

/*
   CBODY2 -> '=' ';' '('
    setting variable or function
*/
static int cbody2(T_symbol *symbol)
{{{
    enter(__func__);
    // '=' or ';' or '(' expected
    if (get_token())
        return leave(__func__, LEX_ERROR);

    // variable
    if (token->type == TT_assign || token->type == TT_semicolon) {

        if (symbol->data_type == is_void) {
            return leave(__func__, TYPE_ERROR);
        }
        // setting symbol as variable
        symbol->symbol_type = is_var;
        symbol->attr.var = calloc(1, sizeof(T_var_symbol));

        if (!(symbol->attr.var)) {
            return leave(__func__, INTERNAL_ERROR);
        }

        // initialize string variable
        if (symbol->data_type == is_str) {
            if ( !(symbol->attr.var->value.str = str_init()) )
                return leave(__func__, INTERNAL_ERROR);
        }

        // just reading everything till `;`
        while (token->type != TT_semicolon && token->type != TT_eof) {
            if (get_token()) {
                return leave(__func__, LEX_ERROR);
            }
        }

        if (token->type == TT_semicolon)
            return leave(__func__, 0);
        return leave(__func__, SYNTAX_ERROR);
    }
    else if (token->type == TT_lBracket) {
        symbol->symbol_type = is_func;
        symbol->attr.func = calloc(1, sizeof(T_func_symbol));

        if (!(symbol->attr.func)) {
            return leave(__func__, INTERNAL_ERROR);
        }

        return func(symbol)+leave(__func__, 0);
    }
    else
        return leave(__func__, SYNTAX_ERROR);

    return leave(__func__, SYNTAX_ERROR);
}}}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
static int func(T_symbol *symbol)
{{{
    enter(__func__);
    int res = 0;

    // creating local symbol table
    symbol->attr.func->local_table = table_init(RANGE);

    if (!(symbol->attr.func->local_table)) {
        return leave(__func__, INTERNAL_ERROR);
    }

    res = par(symbol);
    if (res)
        return leave(__func__, res);

    res = fbody(symbol);

    // printing local table - just for debug
    print_table(symbol->attr.func->local_table);

    if (res)
        return leave(__func__, res);

    return leave(__func__, 0);
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
        return leave(__func__, LEX_ERROR);
    }

    if (token->type == TT_rBracket) {
        // no parameters
        symbol->attr.func->par_count = 0;
        symbol->attr.func->arguments = NULL;
        return leave(__func__, 0);
    }

    token_vector tv = token_vec_init();
    if (!tv) {
        return leave(__func__, INTERNAL_ERROR);
    }

    do {
        if (token_push_back(tv, token)) {
            rc = INTERNAL_ERROR;
            goto free_tvect;
        }

        if (get_token()) {
            rc = LEX_ERROR;
            goto free_tvect;
        }
    } while (token->type != TT_rBracket && token->type != TT_eof);

    // pushing ')'
    if (token_push_back(tv, token)) {
        rc = INTERNAL_ERROR;
        goto free_tvect;
    }


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
        T_symbol *arg_var = calloc(1, sizeof(T_symbol));
        if (!arg_var) {
            rc = INTERNAL_ERROR;
            goto free_args;
        }

        if (!(arg_var->attr.var = calloc(1, sizeof(T_var_symbol)))) {
            rc = INTERNAL_ERROR;
            goto free_args;
        }

        arg_var->symbol_type = is_var;      // is variable
        arg_var->id = tptr->attr.str->buf;  // name of variable
        tptr->attr.str->buf = NULL;         // discredit token buffer
        arg_var->data_type = dtype;         // data type
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
    return leave(__func__, 0);

    // errors
    free_args:
    free(arguments);
    free_tvect:
    token_vec_delete(tv);
    return leave(__func__, rc);
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static int fbody() {{{
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
}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static int st_list() {
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
static int stat() {
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
                        return leave(__func__, SYNTAX_ERROR);

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
                        while (token->type != TT_semicolon && token->type != TT_eof) {
                            if (get_token()) {
                                return leave(__func__, LEX_ERROR);
                            }
                        }
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
                    // reading `)` or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }

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
                    // reading till ')' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }

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
                        return leave(__func__, res);
                    // if next word is else do call st_else()
                    if (get_token())
                        return leave(__func__, LEX_ERROR);
                    if (token->type == TT_keyword && token->attr.keyword == TK_else)
                        return leave(__func__, st_else());
                    // no else read, must unget
                    unget_token();
                    return leave(__func__, 0);
                }
            case TK_return:
                // RET return EXPR;
                // RET return;
                // reading whole expression
                {
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            return leave(__func__, LEX_ERROR);
                        }
                    }

                    if (token->type == TT_eof) {
                         return leave(__func__, SYNTAX_ERROR);
                    }

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
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        if (get_token()) {
                            return leave(__func__, LEX_ERROR);
                        }
                    }
                    if (token->type == TT_eof) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    return leave(__func__, 0);
                }
                else if (token->type == TT_lBracket) {
                    // XXX func1()+func2();
                    // reading whole expression
                    // reading till ';' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        if (get_token()) {
                            return leave(__func__, LEX_ERROR);
                        }
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                        return leave(__func__, SYNTAX_ERROR);
                    }
                    // TODO expression handling
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
            // reading till ';' or 'eof' read
            while ( token->type != TT_semicolon && token->type != TT_eof) {
                if (get_token()) {
                    return leave(__func__, LEX_ERROR);
                }
            }
            if (token->type == TT_eof) {
                return leave(__func__, SYNTAX_ERROR);
            }
            return leave(__func__, 0);
        }
        else if (token->type == TT_lBracket) {
            // id();
            // reading whole expression
            // reading till ';' or 'eof' read
            bc = 0; // bracket counter
            do {
                if (get_token()) {
                    return leave(__func__, LEX_ERROR);
                }
                if (token->type == TT_lBracket)
                    bc++;
                if (token->type == TT_rBracket)
                    bc--;
            } while ( bc != -1 && token->type != TT_eof);

            if (token->type == TT_eof) {
                return leave(__func__, SYNTAX_ERROR);
            }

            if (get_token())
                return leave(__func__, LEX_ERROR);

            if (token->type == TT_semicolon)
                return leave(__func__, 0);

            return leave(__func__, SYNTAX_ERROR);
        }
    }
        return leave(__func__, SYNTAX_ERROR);
}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static int st_else() {
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
static int st_else2() {
    enter(__func__);

    if (get_token()) {
        return leave(__func__, LEX_ERROR);
    }
    // only '('
    if (token->type == TT_lBracket) {
        // reading till ')' or 'eof' read
        int bc = 0; // bracket counter
        do {
            if (get_token()) {
                return leave(__func__, LEX_ERROR);
            }
            if (token->type == TT_lBracket)
                bc++;
            if (token->type == TT_rBracket)
                bc--;
        } while ( bc != -1 && token->type != TT_eof);

        if (token->type == TT_eof) {
            return leave(__func__, SYNTAX_ERROR);
        }

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

    // return value
    int res;

    /*
       lexical + syntax analysis
       filling symbol table + checking redefinitions
    */
    res =  prog();
    if (res)
        goto errors;

    // TODO call again semantic analyser and build instruction list
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

