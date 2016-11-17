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
#include "symbol.h"

// unget token
static bool get_token_flag = false;

#ifdef REC_DEBUG
#define return return leave(__func__, 0) +
#endif

#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
T_symbol_table *symbol_tab;
T_token *token;
ilist *instr_list;
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
static int st_else(T_symbol_table *local_tab);
static int st_else2(T_symbol_table *local_tab);

/*******************************************************
 * function handling helper functions
 *
 */

#define BI_COUNT 9

// including actual token
// excluding ';'
token_vector read_to_semic()
{{{
    token_vector tv = token_vec_init();
    while (token->type != TT_semicolon) {
        token_push_back(tv, token);
        get_token();
    }
    return tv;
}}}

// excluding actual token
// including ')'
token_vector read_to_rbrac()
{{{
    token_vector tv = token_vec_init();
    int bc = 0; // bracket counter
    do {
        get_token();
        // pushing token
        token_push_back(tv, token);
        if (token->type == TT_lBracket)
            bc++;
        if (token->type == TT_rBracket)
            bc--;
    } while ( bc != -1);
    return tv;
}}}

// built-ins
typedef enum {
    b_readI,
    b_readD,
    b_readS,
    b_print,
    b_length,
    b_substr,
    b_compare,
    b_find,
    b_sort
} T_builtins;


// built-ins
static char *arr_ifj16[] = {
    "readInt", "readDouble", "readString", "print",
    "length",  "substr", "compare", "find", "sort"
};


#define is_par(t) (t->type == TT_id || t->type == TT_fullid ||\
                   t->type == TT_string || t->type == TT_int || \
                   t->type == TT_double)
#define is_comma(t) (t->type == TT_comma)
#define is_rbrac(t) (t->type == TT_rBracket)

void check_par_syntax(T_token *it, int tcount, int exp_toks)
{{{

    int state = 0;
    while (tcount >= 0) {
        tcount--;
        exp_toks--;
        switch (state) {
            case 0:
                if (is_par(it))
                    state = 1;
                else if (is_rbrac(it))
                    state = 3;
                else 
                    terminate(SYNTAX_ERROR);
                break;
            case 1:
                if (is_comma(it))
                    state = 2;
                else if(is_rbrac(it))
                    state = 3;
                else
                    terminate(SYNTAX_ERROR);
                break;
            case 2:
                if (is_par(it))
                    state = 1;
                else
                    terminate(SYNTAX_ERROR);
                break;
            case 3:
                if (tcount != exp_toks)
                    terminate(TYPE_ERROR);
                break; 
        }
        it++;
    }

}}}

/**
 *
 * Proceed a build-in functions.
 *
 * @param func_id function name
 * @param tv vector of tokens
 * @param L instruction list
 * @param dest pointer to a symbol in local table on stack, or in global table
 * @return 0 on success, TYPE_ERROR or DEFINITION_ERROR
 *
 */
int handle_builtins(T_token *it, int tcount, ilist *L, T_symbol *dest,
                    T_symbol_table *local_tab)
{{{
    // magic constant 3 - minimal count of tokens
/*    if (tcount < 3)
        terminate(SYNTAX_ERROR);*/
    // getting function name
    char *func_id = strchr(it->attr.str, '.') + 1;
    int i;
    // loop over all ifj.*
    for (i = 0; i < BI_COUNT && strcmp(func_id, arr_ifj16[i]);i++);
    it++;   // '('
    it++;   // ')' or 'parameter'
    switch (i) {
        case b_readI:
        case b_readD:
        case b_readS:
            {{{
                // no parameters
                check_par_syntax(it, tcount - 2, 1);
        /*        if (tcount != 3 || it->type != TT_rBracket) {
                    terminate(TYPE_ERROR);
                }*/
                // a = readInt();
                T_data_type dtype;
                dtype = i == b_readI ? is_int : is_double;
                dtype = i == b_readS ? is_str : dtype;
                if (dest)
                    dtype = dest->attr.var->data_type;
                // XXX double a = readInt
                if (i == b_readI && (dtype == is_int || dtype == is_double )) {
                    create_instr(L, TI_readInt, dest, NULL, NULL);
                }
                else if (i == b_readD && dtype == is_double) {
                    create_instr(L, TI_readDouble, dest, NULL, NULL);
                }
                else if (i == b_readS && dtype == is_str) {
                    create_instr(L, TI_readString, dest, NULL, NULL);
                }
                else
                    terminate(TYPE_ERROR);

                return 0;
            }}}
        case b_print:


        case b_length:
        case b_sort:
            // str (str) | int (str)
            {{{
                check_par_syntax(it, tcount - 2, 2);

                T_symbol *sym;
                T_instr_type ins = i == b_sort ? TI_sort : TI_length;
                // checking destination data type if any
                if (dest) {
                    T_data_type dtype = dest->attr.var->data_type;
                    if ((i == b_sort && dtype != is_str ) || (i == b_length &&
                                (dtype == is_int || dtype == is_double) ))
                    {
                        terminate(TYPE_ERROR);
                    }
                }
                // checking string variable
                if (it->type == TT_id || it->type == TT_fullid) {
                    // checking type and if it was defined
                    sym = is_defined(it->attr.str, local_tab,
                                               actual_class, is_str);
                    create_instr(L, ins, dest, sym, NULL);
                }
                else if (it->type == TT_string) {
                    sym = add_constant(it->attr, symbol_tab, is_str);
                    create_instr(L, ins, dest, sym, NULL);
                }
                else
                    terminate(TYPE_ERROR);

                return 0;
            }}}
        case b_find:
        case b_compare:
            // int (str, str)
            {{{

                check_par_syntax(it, tcount - 2, 4);
                T_symbol *sym1, *sym2;
                // checking data type
                if (dest) {
                    T_data_type dtype = dest->attr.var->data_type;
                    if (dtype == is_int || dtype == is_double)
                    {
                        terminate(TYPE_ERROR);
                    }
                }

                // second argument
                T_token *it2 = it + 2;
                // setting instruction
                T_instr_type ins = (i == b_find ? b_find : b_compare);
                // determining constant or identifier
                if (it->type == TT_id || it->type == TT_fullid) {
                    sym1 = is_defined(it->attr.str, local_tab, actual_class,
                                     is_str);
                }
                else if (it->type == TT_string) {
                    sym1 = add_constant(it->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                if (it2->type == TT_id || it2->type == TT_fullid) {
                    sym2 = is_defined(it2->attr.str, local_tab, actual_class,
                                     is_str);
                }
                else if (it2->type == TT_string) {
                    sym2 = add_constant(it2->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                // creating instruction
                create_instr(L, ins, dest, sym1, sym2);
                return 0;
            }}}
        case b_substr:
            // TODO I need stack
            {{{


                return 0;
            }}}
        default:
            terminate(DEFINITION_ERROR);
    }
    return 0;
}}}

/**
 *
 * Proceed a function.
 *
 * @param it token array from identifier to right bracket.
 * @param tcount number of tokens, should be at least 3
 * @param L instruction list
 * @param dest variable where return value will be stored
 * @param local_tab local symbol table of function
 * @return 0 on success, TYPE_ERROR or DEFINITION_ERROR
 *
 */
int handle_function(T_token *it, unsigned tcount, ilist *L, T_symbol *dest,
                    T_symbol_table *local_tab)
{{{
    // table_find is able to derive from ifj16.readInt pointer to class ifj16
    T_symbol *sym = table_find(symbol_tab, it->attr.str, actual_class);
    // found ifj16
    if (!strcmp(sym->id, "ifj16")) {
        // handle builtins
        handle_builtins(it, tcount, instr_list, dest, local_tab);
        return 0;
    }
    // no such function found
    if (!sym || sym->symbol_type != is_func)
        terminate(DEFINITION_ERROR);

    // function found

    unsigned pcount = sym->attr.func->par_count;

    // type control
    if (dest) {
        T_data_type d1 = sym->attr.func->data_type;
        T_data_type d2 = dest->attr.var->data_type;
        if ( d1 != d2 && (d1 != is_int || d2 != is_double ))
            terminate(TYPE_ERROR);
    }

    // parameters
    T_symbol **pars = (T_symbol**)sym->attr.func->arguments;
    // handling parameters
    it++;it++;  // skipping function id and '('
    unsigned exp_tc = pcount > 1 ? 2*pcount : pcount + 1;
    check_par_syntax(it, tcount - 2, exp_tc);

    for (unsigned j = 0; j < pcount;j++) {
        // parameter
        T_symbol *sym;
        // data type of each parameter
        T_data_type dtype = pars[j]->attr.var->data_type;

        if (it->type == TT_id || it->type == TT_fullid) {
            // check the identifier
            sym = is_defined(it->attr.str, local_tab, actual_class, dtype);
            // push identifier on stack
        }
        else if (it->type == TT_string && dtype == is_str) {
            sym = add_constant(it->attr, symbol_tab, is_str);
        }
        else if (it->type == TT_int && dtype == is_int) {
            sym = add_constant( it->attr, symbol_tab, is_int);
        }
        else if ((it->type == TT_int || it->type == TT_double)
                  && dtype == is_double)
        {
            sym = add_constant( it->attr, symbol_tab, is_double);
        }
        else {
            terminate(TYPE_ERROR);
        }

        create_instr(L, TI_push_param, sym, NULL, NULL);
   }
   // TODO insert instruction CALL
    return 0;
}}}

bool check_if_func(T_token *it)
{{{
    return ((it->type == TT_id || it->type == TT_fullid) &&
                (it+1)->type == TT_lBracket);
}}}

/******************************************************
 * semantic analysis
 *
 */
// PROG -> BODY eof
static int prog()
{{{
    enter(__func__);

    return  body();
}}}

// BODY -> CLASS BODY
// BODY -> e
static int body()
{{{
    enter(__func__);
    unsigned res = 0;   // return value
    get_token();

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        res = class();
        if (res)
            return res;

        return body();
    }
    else if (token->type == TT_eof) {
        return  0;
    }

    return SYNTAX_ERROR;
}}}

/* CLASS -> id lb CBODY rb
*/
static int class()
{{{
    enter(__func__);
    // going for id
    get_token();

    // already tested in by first throughpass
    actual_class = table_find(symbol_tab, token->attr.str, NULL);

    // going for '{'
    get_token();

    return cbody();
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
        T_symbol *s = table_find(symbol_tab,token->attr.str, actual_class);
        if (s->symbol_type == is_func) {
            // updating actual function
            actual_func = s;
            // local table
            T_symbol_table *local_tab = s->attr.func->local_table;
            res = cbody2(local_tab);
        }
        else {
            res = cbody2(NULL); // if variable - no local table
        }

        if (res)
            return res;

        return cbody();
    }
    else
        return 0;
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

        // cannot be called function XXX
        token_vector tv = read_to_semic();
        // call precedence
        token_vec_delete(tv);
        return 0;
    }
    else {
        return func(local_tab);
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
    // just for debug
    print_function(actual_func);
    return res;
}}}

/*
   just read till ')'
*/
static int par()
{{{
    enter(__func__);
    while (token->type != TT_rBracket)
        get_token();
    return 0;
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static int fbody(T_symbol_table *local_tab)
{{{
    enter(__func__);
    get_token();

    // XXX maybe semicolon makes syntax error ?
    if (token->type == TT_semicolon) {
        return 0;
    }
    else {
        return st_list(local_tab);
    }

}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static int st_list(T_symbol_table *local_tab)
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
        token->type == TT_fullid )
    {

        int res = stat(local_tab);
        if (res)
            return res;

        return st_list(local_tab);
    }
    else if (token->type == TT_rCurlBracket) {
        return 0;
    }
    else if (token->type == TT_lCurlBracket) {
        int res = st_list(local_tab);        // { ST-LIST } ST-LIST
        if (res)
            return res;
        return st_list(local_tab);
    }
    return SYNTAX_ERROR;
}}}

// STAT -> many...
static int stat(T_symbol_table *local_tab)
{{{
    enter(__func__);
    // while|for|if|return|continue|break|types
    if (token->type == TT_keyword) {
        switch(token->attr.keyword) {
            case TK_int:
            case TK_double:
            case TK_String:
                {{{
                    // rule: STAT -> TYPE id ;| = EXPR ;

                    unsigned dtype = token->attr.keyword;   // data type
                    // increasing local variable count
                    actual_func->attr.func->local_count++;

                    // id in token
                    get_token();
                    if (table_find(local_tab, token->attr.str, NULL)) {
                        // redefinition
                        return DEFINITION_ERROR;
                    }

                    // creating a new variable symbol
                    T_symbol *sym = create_var(token->attr.str, dtype);
                    // discredit free call on token
                    token->attr.str = NULL;

                    // inserting to table
                    table_insert(local_tab, sym);

                    // ';' or '='
                    get_token();

                    if (token->type == TT_semicolon) {
                        return 0;
                    }
                    else {
                        // assign '='

                        // creating token vector
                        get_token();   // necessary, will be included in vector
                        token_vector tv = read_to_semic();
                        // id ( )
                        if (tv->last > 3 && check_if_func(tv->arr) ) {
                            // handling function
                            handle_function(tv->arr, tv->last, instr_list, sym,
                                            local_tab);
                        }
                        else {
                            // TODO precedence analysis
                        }

                        token_vec_delete(tv);
                        return 0;
                    }
                }}}
            case TK_while:
            case TK_if:
                {{{
                    T_keyword keyword = token->attr.keyword;
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    get_token();

                    // reading till `)`
                    // '(' is not included
                    token_vector tv = read_to_rbrac();
                    // TODO send tvect to precedence analyser
                    // XXX last token in tvect is ')'

                    token_vec_delete(tv);
                    get_token();    // '{'
                    // if (..) { ....
                    // beginning new statement list
                    int res;
                    res = st_list(local_tab);
                    if (res)
                        return res;
                    // TODO insert different instruction while/if
                    if (keyword == TK_while) {
                        return 0;
                    }
                    // if next word is else do call st_else()
                    get_token();

                    if (token->type == TT_keyword && token->attr.keyword == TK_else)
                        return st_else(local_tab);
                    // no else read, must unget
                    unget_token();
                    return 0;
                }}}
            case TK_return:
                {{{
                    // RET return EXPR;
                    // RET return;
                    // reading whole expression
                    get_token();
                    // excluding return
                    token_vector tv = read_to_semic();
                    T_data_type dtype = actual_func->attr.func->data_type;
                    // return expression in void function or vice versa
                    if ( (tv->last == 0 && dtype != is_void) ||
                         (tv->last != 0 && dtype == is_void) )
                    {
                        return TYPE_ERROR;
                    }
                    // TODO call expression handler

                    token_vec_delete(tv);
                    return 0;
                }}}
            default:
                return SYNTAX_ERROR;
        }
    }
    // id/fullid = exp | fullid()/id()
    else
    {{{
        // reading all tokens till ';', ';' is not included in vector
        token_vector tv = read_to_semic();
        // id = expr
        // id ( pars )
        // minimal count of tokens
        if (tv->last < 3)
            return SYNTAX_ERROR;
        // token iterator
        T_token *it = tv->arr;

        char *iden = it->attr.str;       // identifier name
        T_symbol *loc_sym, *glob_sym;    // local and global symbol

        // checking whether it was defined
        loc_sym = table_find_simple(local_tab, iden, NULL);    // local var
        glob_sym = table_find(symbol_tab, iden, actual_class); // static

        // undefined reference
        if (!loc_sym && !glob_sym ) {
           return DEFINITION_ERROR;
        }
        // getting '=' or '('
        it++;
        if (it->type == TT_assign)
        {{{
            // id = ....;
            T_symbol *sym = loc_sym ? loc_sym : glob_sym;
            // assignment goes to function or class
            if (!sym || sym->symbol_type != is_var) {
                return DEFINITION_ERROR;
            }
            // go after expression or function
            it++;
            // id = id ( )
            if (tv->last >= 5 && check_if_func(it) ) {
                // token count
                unsigned tcount = tv->last - 2;
                handle_function(it, tcount, instr_list, sym, local_tab);
            }
            else {
                // TODO precedence analysis
            }
            token_vec_delete(tv);
            return 0;
        }}}
        else
        {{{
            handle_function(tv->arr, tv->last, instr_list, NULL, local_tab);
            token_vec_delete(tv);
            return 0;
        }}}
    }}}
}}}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static int st_else(T_symbol_table *local_tab)
{{{
    enter(__func__);

    get_token();
    // `else {`
    if (token->type == TT_lCurlBracket) {
        int res = st_list(local_tab);
        return res;
    }
    // `else if`
    else {
        return st_else2(local_tab);
    }

}}}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
static int st_else2(T_symbol_table *local_tab)
{{{
    enter(__func__);

    // '(' read
    get_token();
    // reading till ')'
    token_vector tv = read_to_rbrac();
    // TODO precedence
    token_vec_delete(tv);
    // {
    get_token();
    // beginning of statement list
    int res;
    res = st_list(local_tab);
    if (res)
        return res;
    // if next word is else do call st_else()
    get_token();

    if (token->type == TT_keyword && token->attr.keyword == TK_else)
        return st_else(local_tab);
    // no else, token must be returned
    unget_token();
    return 0;
}}}

int second_throughpass() {
    int res = prog();
    print_table(symbol_tab);
    return res;
}

