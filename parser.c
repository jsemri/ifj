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
#include "ilist.h"
#include "interpret.h"
#include "precedence_analyser.h"


#ifdef DEBUG
#define return return leave(__func__, 0) +
#endif

// unget token
static bool get_token_flag = false;
#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
ilist *glist;
T_symbol_table *symbol_tab;
T_token *token;
FILE *source;
static int in_block = 0;
// pointer to actual class
static T_symbol *actual_class;

// each function represents a nonterminal symbol in LL(1) table
static void prog();
static void body();
static void class();
static void cbody();
static void cbody2(char *iden, T_data_type dtype);
static void func(char *iden, T_data_type dtype);
static void fbody();
static void par(T_symbol *symbol);
static void st_list();
static void stat();
static void st_else();
static void st_else2();

// PROG -> BODY eof
static void prog()
{{{
    enter(__func__);
    body();
}}}

// BODY -> CLASS BODY
// BODY -> e
static void body()
{{{

    enter(__func__);
    // must read, because of recursion... (or not ?)
    get_token();

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        class();

        body();
    }
    else if (token->type == TT_eof) {
        return;
    }
    else {
        terminate(SYNTAX_ERROR);
    }
}}}

/* CLASS -> id lb CBODY rb
   checking syntax
   inserting symbol `class`
   changing actual class poiter
*/
static void class()
{{{
    enter(__func__);
    // going for id
    get_token();

    // `id` expected
    if (token->type != TT_id) {
        terminate(SYNTAX_ERROR);
    }

    T_symbol *symbol;
    // finding a symbol
    if ((symbol = table_find(symbol_tab, token->attr.str, NULL))) {
        // class with same name found - definition error
        terminate(DEFINITION_ERROR);
    }

    symbol = create_symbol(token->attr.str, is_class);
    token->attr.str = NULL;        // no deep copy of char*
    table_insert(symbol_tab, symbol);

    actual_class = symbol;

    // going for '{'
    get_token();

    if (token->type != TT_lCurlBracket) {
        terminate(SYNTAX_ERROR);
    }

    // cbody should return } in token
    cbody();
}}}

/*
   CBODY -> static TYPE id CBODY2 CBODY
   symbol data type, member class, id defined
*/
static void cbody()
{{{
    enter(__func__);
    unsigned dtype;     // data type
    // static or '}'
    get_token();

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        get_token();

        // 'data type' expected
        if (token->type == TT_keyword && token->attr.keyword < TK_boolean) {
            dtype = token->attr.keyword;
        }
        else {
            terminate(SYNTAX_ERROR);
        }

        // 'id' expected
        get_token();

        if (token->type != TT_id){
            terminate(SYNTAX_ERROR);
        }

        // setting identifier name
        char *iden = token->attr.str;
        // finding a symbol
        if (table_find(symbol_tab, token->attr.str, actual_class)) {
            // id in class with same name found - definition error
            terminate(DEFINITION_ERROR);
        }
        // discrediting
        token->attr.str = NULL;

        cbody2(iden, dtype);

        cbody();
    }
    else if (token->type != TT_rCurlBracket)
        terminate(SYNTAX_ERROR);    // XXX
}}}

/*
   CBODY2 -> '=' ';' '('
    setting variable or function
*/
static void cbody2(char *iden, T_data_type dtype)
{{{
    enter(__func__);
    // '=' or ';' or '(' expected
    get_token();

    // variable
    if (token->type == TT_assign || token->type == TT_semicolon) {

        if (dtype == is_void) {
            terminate(TYPE_ERROR);
        }
        // creating symbol as variable
        T_symbol *sym = create_var(iden, dtype);
        sym->member_class = actual_class;
        table_insert(symbol_tab, sym);
        if (token->type == TT_semicolon)
            return;

        // just reading everything till `;`
        token_vector tv = token_vec_init();
        while (token->type != TT_semicolon && token->type != TT_eof) {
            get_token();
            token_push_back(tv, token);
        }

        // id = ; or no ';'
        if (tv->last == 1 || token->type != TT_semicolon )
            terminate(SYNTAX_ERROR);

        precedence_analyser(tv->arr, tv->last-1, sym, symbol_tab, actual_class,
                            glist);

        token_vec_delete(tv);

    }
    else if (token->type == TT_lBracket) {
        // function
        func(iden, dtype);
    }
    else
        terminate(SYNTAX_ERROR);
}}}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
static void func(char *iden, T_data_type dtype)
{{{
    enter(__func__);
    // creating a function
    T_symbol *sym = create_func(iden, dtype);
    sym->member_class = actual_class;
    table_insert(symbol_tab, sym);

    par(sym);
    fbody();
}}}

/*
    PAR -> ε | type id ...
*/
static void par(T_symbol *symbol)
{{{
    enter(__func__);
    // reading for ')' or type
    get_token();

    if (token->type == TT_rBracket) {
        // no parameters
        symbol->attr.func->par_count = 0;
        symbol->attr.func->arguments = NULL;
        return;
    }

    token_vector tv = token_vec_init();

    do {
        token_push_back(tv, token);
        get_token();
    } while (token->type != TT_rBracket && token->type != TT_eof);

    // pushing ')'
    token_push_back(tv, token);

    if (token->type == TT_eof) {
        terminate(SYNTAX_ERROR);
    }

    T_func_symbol *fptr = symbol->attr.func;
    // number of read tokens
    if ((tv->last)%3) {
        terminate(SYNTAX_ERROR);
    }
    fptr->par_count = (tv->last)/3;

    // allocating pointer to arguments
    void **arguments = calloc(1,sizeof(void*)*fptr->par_count);
    if (!arguments) {
        terminate(SYNTAX_ERROR);
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
                terminate(TYPE_ERROR);
        }
        else {
            free(arguments);
            terminate(SYNTAX_ERROR);
        }
        // identifier expected
        tptr++;
        if (tptr->type != TT_id) {
            free(arguments);
            terminate(SYNTAX_ERROR);
        }
        // argument names must differ
        if (table_find(fptr->local_table, tptr->attr.str, NULL)) {
            free(arguments);
            terminate(DEFINITION_ERROR);
        }

        // create and insert variable symbol
        T_symbol *arg_var = create_var(tptr->attr.str, dtype);
        tptr->attr.str = NULL;         // discredit token buffer

        arguments[i++] = arg_var;

        // inserting variable
        table_insert(fptr->local_table, arg_var);

        // skipping ','
        tptr++;
        if (tptr->type == TT_comma) {
            tptr++;
        }
        else if (tptr->type != TT_rBracket) {
            free(arguments);
            terminate(SYNTAX_ERROR);
        }
    }

    fptr->arguments = arguments;
    token_vec_delete(tv);
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static void fbody()
{{{
    enter(__func__);
    get_token();

    if (token->type != TT_lCurlBracket) {
        terminate(SYNTAX_ERROR);
    }

    st_list();
}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static void st_list()
{{{
    enter(__func__);
    // read only if '{' or ';'
    if (token->type == TT_lCurlBracket || token->type == TT_rCurlBracket ||
        token->type == TT_semicolon || token->type == TT_keyword) {
        // expecting whatever what is statement
        get_token();
    }
    // keyword
    if (token->type == TT_keyword || token->type == TT_id ||
        token->type == TT_fullid)
    {
        stat();
        st_list();
    }
    else if (token->type == TT_rCurlBracket) {
        return;
    }
    else if (token->type == TT_lCurlBracket) {
        in_block++;
        st_list();        // { ST-LIST } ST-LIST
        in_block--;
        st_list();
    }
    else {
        terminate(SYNTAX_ERROR);
    }
}}}

// STAT -> many...
static void stat()
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
                        terminate(SYNTAX_ERROR);

                    get_token();

                    if (token->type != TT_id) {
                        terminate(SYNTAX_ERROR);
                    }
                    get_token();

                    if (token->type == TT_semicolon) {
                        return;
                    }
                    else if (token->type == TT_assign) {
                        while (token->type != TT_semicolon && token->type != TT_eof) {
                            get_token();

                        }
                        if (token->type == TT_semicolon) {
                            return;
                        }
                        terminate(SYNTAX_ERROR);
                    }
                    else {
                        terminate(SYNTAX_ERROR);
                    }
                }
           case TK_while:
           case TK_if:
                {
                    // while or if
                    T_keyword keyword = token->attr.keyword;
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    get_token();

                    if (token->type != TT_lBracket) {
                        terminate(SYNTAX_ERROR);
                    }
                    // reading till ')' or 'eof' read
                    bc = 0; // bracket counter
                    do {
                        get_token();
                        if (token->type == TT_lBracket)
                            bc++;
                        if (token->type == TT_rBracket)
                            bc--;
                    } while ( bc != -1 && token->type != TT_eof);

                    if (token->type == TT_eof) {
                         terminate(SYNTAX_ERROR);
                    }

                    get_token();

                    // if (..) { ....
                    if (token->type != TT_lCurlBracket) {
                        terminate(SYNTAX_ERROR);
                    }
                    // beginning new statement list
                    in_block++;
                    st_list();
                    in_block--;
                    // end while
                    if (keyword == TK_while) {
                        return;
                    }

                    // if next word is else do call st_else()
                    get_token();
                    if (token->type == TT_keyword &&
                        token->attr.keyword == TK_else)
                    {
                        st_else();
                    }
                    else {
                        // no else read, must unget
                        unget_token();
                    }
                    return;
                }
            case TK_return:
                // RET return EXPR;
                // RET return;
                // reading whole expression
                {
                    // reading till ';' or 'eof' read
                    while ( token->type != TT_semicolon && token->type != TT_eof) {
                        get_token();
                    }

                    if (token->type == TT_eof) {
                         terminate(SYNTAX_ERROR);
                    }

                    return;
                }
            default:
                terminate(SYNTAX_ERROR);
        }
    }
    // id = exp
    else if (token->type == TT_id || token->type == TT_fullid) {

        // inline rule
        get_token();

        if (token->type == TT_assign) {
             // reading till ';' or 'eof' read
             while ( token->type != TT_semicolon && token->type != TT_eof) {
                 get_token();

             }
             if (token->type == TT_eof) {
                 terminate(SYNTAX_ERROR);
             }
             return;
         }
         else if (token->type == TT_lBracket) {
             // id();
             // reading whole expression
             // reading till ';' or 'eof' read
             bc = 0; // bracket counter
             do {
                 get_token();

                 if (token->type == TT_lBracket)
                     bc++;
                 if (token->type == TT_rBracket)
                     bc--;
             } while ( bc != -1 && token->type != TT_eof);

             if (token->type == TT_eof) {
                 terminate(SYNTAX_ERROR);
             }

             get_token();

             if (token->type == TT_semicolon)
                 return;

             terminate(SYNTAX_ERROR);
         }
    }
    terminate(SYNTAX_ERROR);
}}}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static void st_else()
{{{
    enter(__func__);

    get_token();

    // `else {`
    if (token->type == TT_lCurlBracket) {
        in_block++;
        st_list();
        in_block--;
    }
    // `else if`
    else if (token->type == TT_keyword && token->attr.keyword == TK_if) {
        st_else2();
    }
    else {
        terminate(SYNTAX_ERROR);
    }
}}}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
static void st_else2()
{{{
    enter(__func__);

    get_token();

    // only '('
    if (token->type == TT_lBracket) {
        // reading till ')' or 'eof' read
        int bc = 0; // bracket counter
        do {
            get_token();

            if (token->type == TT_lBracket)
                bc++;
            if (token->type == TT_rBracket)
                bc--;
        } while ( bc != -1 && token->type != TT_eof);

        if (token->type == TT_eof) {
            terminate(SYNTAX_ERROR);
        }

        get_token();

        // begining of statement list
        if (token->type != TT_lCurlBracket) {
            terminate(SYNTAX_ERROR);
        }
        in_block++;
        st_list();
        in_block--;
        // if next word is else do call st_else()
        get_token();
        if (token->type == TT_keyword && token->attr.keyword == TK_else) {
            st_else();
        }
        else {
            // no else, token must be returned
            unget_token();
        }
    }
    else {
        terminate(SYNTAX_ERROR);
    }
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

    if(!(glist = list_init())) {
        token_free(&token);
        table_remove(&symbol_tab);
        return INTERNAL_ERROR;
    }

    // insert ifj16 class
    fill_ifj16();

    /*
       lexical + syntax analysis
       filling symbol table + checking redefinitions
    */
    prog();
    // checking Main class and run() function
    T_symbol *Mainclass = table_find(symbol_tab, "Main", NULL);
    if (!Mainclass) {
        terminate(SEMANTIC_ERROR);  // definition error? XXX
    }

    T_symbol *run_func = table_find(symbol_tab, "run", Mainclass);
    if (!run_func || run_func->symbol_type != is_func) {
        terminate(SEMANTIC_ERROR);  // definition error? XXX
    }

    if (fseek(source, 0, SEEK_SET)) {
        terminate(INTERNAL_ERROR);
    } else {
        #ifdef REC_DEBUG
        puts("*********");
        #endif
        second_throughpass();
    }

    interpret(run_func);

    table_remove(&symbol_tab);
    token_free(&token);
    list_free(&glist);
    return 0;
}}}

