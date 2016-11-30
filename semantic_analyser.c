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
#include "ilist.h"
#include <assert.h>
#include "interpret.h"
#include "precedence_analyser.h"

// unget token
static bool get_token_flag = false;

#ifdef REC_DEBUG
#define return return leave(__func__, 0) +
#endif

#define get_token()  (get_token_flag ? (get_token_flag = false) : get_token())
#define unget_token() get_token_flag = true

// global variables
ilist *glist;
T_symbol_table *symbol_tab;
T_symbol *acc;
T_token *token;
// pointer to actual class
static T_symbol *actual_class;
static T_symbol *actual_func;

// for debug
int row;
const char *fun;
int part;

// each function represents a nonterminal symbol in LL(1) table
static void prog();
static void body();
static void class();
static void cbody();
static void cbody2();
static void func();
static void fbody();
static void par();
static void st_list();
static void stat(T_symbol_table *local_tab, ilist *instr_list);
static void st_else(T_symbol_table *local_tab, ilist *instr_list);
static void st_else2(T_symbol_table *local_tab, ilist *instr_list);

/*******************************************************
 * function handling helper functions
 *
 */

// count of built-in function
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

#define is_const(t) (t->type == TT_string || t->type == TT_int || \
                     t->type == TT_double)

#define is_iden(t) (t->type == TT_id || t->type == TT_fullid)
#define is_comma(t) (t->type == TT_comma)
#define is_rbrac(t) (t->type == TT_rBracket)
#define is_lbrac(t) (t->type == TT_lBracket)
#define is_plus(t) (t->type == TT_plus)
#define is_real(t) (t->type == TT_double)
#define is_integer(t) (t->type == TT_int )

void check_par_syntax(T_token *it, int tcount, int exp_toks)
{{{
    fun = __func__;
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
    fun = __func__;
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
                // a = readInt();
                T_data_type dtype;
                if (dest) {
                    dtype = dest->attr.var->data_type;
                }
                else {
                    dtype = i == b_readI ? is_int : is_double;
                    dtype = i == b_readS ? is_str : dtype;
                }

                if (i == b_readI && (dtype == is_int || dtype == is_double)) {
                    create_instr(L, TI_readInt, NULL, NULL, dest);
                }
                else if (i == b_readD && dtype == is_double) {
                    create_instr(L, TI_readDouble, NULL, NULL, dest);
                }
                else if (i == b_readS && dtype == is_str) {
                    create_instr(L, TI_readString, NULL, NULL, dest);
                }
                else
                    terminate(TYPE_ERROR);

                return 0;
            }}}
        case b_print:
            {{{
                // token count excluding `id (`
                tcount--;
                T_symbol *sym = 0;
                // print() - no parameters
                if (is_rbrac(it))
                    terminate(TYPE_ERROR);   // FIXME DEF_ERROR ???

                bool is_atleast_one_str;
                int count = 0;
                it = it - 2 + tcount; // going form last
                tcount-=2;
                // last is not ')'
                if (!is_rbrac(it))
                    terminate(SYNTAX_ERROR);
                it--;
                while (tcount > 0 ) {
                    if (is_iden(it)) {
                        sym = is_defined(it->attr.str, local_tab,
                                               actual_class, is_void);
                    }
                    else if (is_const(it)) {
                        T_data_type dtype = is_real(it) ? is_double : is_int;
                        dtype = (it->type == TT_string) ? is_str : dtype;
                        sym = add_constant(&it->attr, symbol_tab, dtype);
                    }
                    else if (it->type == TT_bool) {
                        sym = add_constant(&it->attr, symbol_tab, is_bool);
                    }
                    else {
                        terminate(SYNTAX_ERROR);
                    }
                    count++;
                    // at least one parameter has to be string
                    if (sym->attr.var->data_type == is_str)
                        is_atleast_one_str = true;

                    create_instr(L, TI_push_var, sym, NULL, NULL);
                    it--;
                    tcount--;
                    // token = '+' and is not last
                    if (is_plus(it) && tcount > 1) {
                        tcount--;
                        it--;
                    }
                    else if (is_lbrac(it) && tcount == 0) {
                        break;
                    }
                    else {
                        terminate(SYNTAX_ERROR);
                    }
                }
                // at least one parameter has to be string
                // if more parameters
                if (!is_atleast_one_str && count > 1)
                    terminate(TYPE_ERROR);

                //if (!is_rbrac(it))
                  //  terminate(SYNTAX_ERROR);
                // add number of parameters
                T_value val;
                val.n = count;
                create_instr(L, TI_print, add_constant(&val, symbol_tab, is_int),
                             NULL, NULL);
                return 0;
            }}}
        case b_length:
        case b_sort:
            // str (str) | int (str)
            {{{
                check_par_syntax(it, tcount - 2, 2);
                T_symbol *sym = 0;
                T_instr_type ins = i == b_sort ? TI_sort : TI_length;
                // checking destination data type if any
                if (dest) {
                    T_data_type dtype = dest->attr.var->data_type;
                    if ((i == b_sort && dtype != is_str ) ||
                        (i == b_length && dtype == is_str ))
                    {
                        terminate(TYPE_ERROR);
                    }
                }
                // checking string variable
                if (is_iden(it)) {
                    // checking type and if it was defined
                    sym = is_defined(it->attr.str, local_tab,
                                               actual_class, is_str);
                }
                else if (it->type == TT_string) {
                    sym = add_constant(&it->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                create_instr(L, ins, sym, NULL, dest);
                return 0;
            }}}
        case b_find:
        case b_compare:
            // int (str, str)
            {{{

                check_par_syntax(it, tcount - 2, 4);
                T_symbol *sym1 = 0;
                T_symbol *sym2 = 0;
                // checking data type
                if (dest) {
                    T_data_type dtype = dest->attr.var->data_type;
                    if (dtype == is_str)
                    {
                        terminate(TYPE_ERROR);
                    }
                }

                // second argument
                T_token *it2 = it + 2;
                // setting instruction
                T_instr_type ins = (i == b_find ? TI_find : TI_compare);
                // determining constant or identifier
                if (is_iden(it)) {
                    sym1 = is_defined(it->attr.str, local_tab, actual_class,
                                     is_str);
                }
                else if (it->type == TT_string) {
                    sym1 = add_constant(&it->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                if (is_iden(it2)) {
                    sym2 = is_defined(it2->attr.str, local_tab, actual_class,
                                      is_str);
                }
                else if (it2->type == TT_string) {
                    sym2 = add_constant(&it2->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                // creating instruction
                create_instr(L, ins, sym1, sym2, dest);
                return 0;
            }}}
        case b_substr:
            {{{
                // str (str|int|int)
                check_par_syntax(it, tcount - 2, 6);
                T_symbol *sym1 = 0;
                T_symbol *sym2 = 0;
                // checking data type
                if (dest) {
                    T_data_type dtype = dest->attr.var->data_type;
                    if (dtype != is_str)
                    {
                        terminate(TYPE_ERROR);
                    }
                }
                // second argument
                T_token *it2 = it + 2;
                // third argument
                T_token *it3 = it + 4;

                // first parameter - must be string
                // determining constant or identifier
                if (is_iden(it)) {
                    sym1 = is_defined(it->attr.str, local_tab, actual_class,
                                     is_str);
                }
                else if (it->type == TT_string) {
                    sym1 = add_constant(&it->attr, symbol_tab, is_str);
                }
                else
                    terminate(TYPE_ERROR);

                // second parameter - must be int
                if (is_iden(it2)) {
                    sym2 = is_defined(it2->attr.str, local_tab, actual_class,
                                     is_int);
                }
                else if (is_integer(it2)) {
                    sym2 = add_constant(&it2->attr, symbol_tab, is_int);
                }
                else
                    terminate(TYPE_ERROR);

                // third parameter - must be int
                T_symbol *sym3 = 0;
                if (is_iden(it3)) {
                    sym3 = is_defined(it3->attr.str, local_tab, actual_class,
                                     is_int);
                }
                else if (is_integer(it3)) {
                    sym3 = add_constant(&it3->attr, symbol_tab, is_int);
                }
                else
                    terminate(TYPE_ERROR);

                // pushing last parameter
                create_instr(L, TI_push_var, sym3, NULL, NULL);
                // creating instruction
                create_instr(L, TI_substr, sym1, sym2, dest);
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
int handle_function(T_token *it, int tcount, ilist *L, T_symbol *dest,
                    T_symbol_table *local_tab)
{{{
    fun = __func__;
    // table_find is able to derive from ifj16.readInt pointer to class ifj16
    T_symbol *fsym = table_find(symbol_tab, it->attr.str, actual_class);

    // not fount
    if (!fsym)
        terminate(DEFINITION_ERROR);

    // found ifj16
    if (!strcmp(fsym->id, "ifj16")) {
        // handle builtins
        handle_builtins(it, tcount, L, dest, local_tab);
        return 0;
    }

    // no such function found
    if (fsym->symbol_type != is_func)
        terminate(DEFINITION_ERROR);
    // function found

    int pcount = fsym->attr.func->par_count;

    // type control
    if (dest && fsym->attr.func->data_type != is_void) {
        T_data_type d1 = fsym->attr.func->data_type;
        T_data_type d2 = dest->attr.var->data_type;
        // d1 == d2 || (d1 = in_int && d2 = is_double)
        if ( d1 != d2 && (d1 != is_int  || d2 !=is_double) )
            terminate(TYPE_ERROR);
    }

    // parameters
    T_symbol **pars = (T_symbol**)fsym->attr.func->arguments;
    // handling parameters
    int exp_tc = pcount > 1 ? 2*pcount : pcount + 1;
    check_par_syntax(it + 2, tcount - 2, exp_tc);
    T_instr *return_label = instr_init(TI_lab,0,0,0);
    // pushing return label
    create_instr(L, TI_push, return_label,0,0);

    it += tcount - 1;
    tcount =-2;it--;
    for (int j = pcount-1; j >= 0;j--) {
        // parameter
        T_symbol *sym = 0;
        // data type of each parameter
        T_data_type dtype = pars[j]->attr.var->data_type;

        if (it->type == TT_id || it->type == TT_fullid) {
            // check the identifier
            sym = is_defined(it->attr.str, local_tab, actual_class, dtype);
            // push identifier on stack
        }
        else if (it->type == TT_string && dtype == is_str) {
            sym = add_constant(&it->attr, symbol_tab, is_str);
        }
        else if (is_integer(it) && dtype == is_int) {
            sym = add_constant(&it->attr, symbol_tab, is_int);
        }
        else if (is_integer(it) && dtype == is_double)
        {
            sym = add_constant(&it->attr, symbol_tab, is_int);
        }
        else if (is_real(it) && dtype == is_double){
            sym = add_constant(&it->attr, symbol_tab, is_double);
        }
        else {
            terminate(TYPE_ERROR);
        }
        create_instr(L, TI_push_var, sym, NULL, NULL);
        // assign return value of function
        // this value ought to be in special symbol
        it-=2;
    }

    create_instr(L, TI_call, fsym, 0, 0);

    list_insert_last(L, return_label);

    if (dest) {
        create_instr(L, TI_mov, acc, NULL, dest);
    }

    // inserting label, where to return from function
    // this instruction should be in stack
    //list_insert_last(L, return_label);
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
static void prog()
{{{
    fun = __func__;

    body();
}}}

// BODY -> CLASS BODY
// BODY -> e
static void body()
{{{
    fun = __func__;
    get_token();

    // `class` or `eof` expected
    if (token->type == TT_keyword && token->attr.keyword == TK_class ) {
        class();
        body();
    }
}}}

/* CLASS -> id lb CBODY rb
*/
static void class()
{{{
    fun = __func__;
    // going for id
    get_token();

    // already tested in by first throughpass
    actual_class = table_find(symbol_tab, token->attr.str, NULL);

    // going for '{'
    get_token();

    cbody();
}}}

/*
   CBODY -> static TYPE id CBODY2 CBODY
*/
static void cbody()
{{{
    fun = __func__;
    // static or '}'
    get_token();

    // 'static' expected
    if (token->type == TT_keyword && token->attr.keyword == TK_static) {
        // reading type
        get_token();

        // 'id' expected
        get_token();

        actual_func = table_find(symbol_tab,token->attr.str, actual_class);
        cbody2();

        cbody();
    }
}}}

/*
   CBODY2 -> '=' ';' '('
*/
static void cbody2()
{{{
    fun = __func__;
    // '=' or ';' or '(' expected
    get_token();

    // variable
    if (token->type == TT_assign) {

        actual_func->attr.var->is_def = true;
        token_vector tv = read_to_semic();
        part = 2;
        precedence_analyser(tv->arr+1, tv->last-1, actual_func, symbol_tab, actual_class,
                            glist);
        part = 0;
        token_vec_delete(tv);
    }
    else if (token->type == TT_semicolon) {
        actual_func->attr.var->is_def = true;
        return;
    }
    else {
        func();
    }
}}}

/* FUNC -> ( PAR ) FBODY
 '(' has been read
*/
static void func()
{{{
    fun = __func__;
    // just read parameters
    par();
    // process body
    fbody();
    // just for debug
//    print_function(actual_func);
}}}

/*
   just read till ')'
*/
static void par()
{{{
    fun = __func__;
    while (token->type != TT_rBracket)
        get_token();
}}}

// FBODY -> { ST_LIST }
// FBODY -> ;
static void fbody()
{{{
    fun = __func__;
    get_token();

    st_list();
}}}

// ST_LIST -> ε
// ST_LIST -> {STLIST} STLIST
// ST_LIST -> STAT STLIST
static void st_list()
{{{
    fun = __func__;
    get_token();
    // keyword
    if (token->type == TT_keyword || token->type == TT_id ||
        token->type == TT_fullid )
    {
        stat(actual_func->attr.func->local_table,
             actual_func->attr.func->func_ilist);

        st_list();
    }
    else if (token->type == TT_rCurlBracket) {
        return;
    }
    else if (token->type == TT_lCurlBracket) {
        st_list();        // { ST-LIST } ST-LIST
        st_list();
    }
}}}

// STAT -> many...
static void stat(T_symbol_table *local_tab, ilist *instr_list)
{{{
    fun = __func__;
    // while|for|if|return|continue|break|types
    if (token->type == TT_keyword) {
        switch(token->attr.keyword) {
            case TK_int:
            case TK_double:
            case TK_String:
            case TK_boolean:
                {{{
                    // rule: STAT -> TYPE id ;| = EXPR ;

                    unsigned dtype = token->attr.keyword;   // data type
                    // increasing local variable count

                    // id in token
                    get_token();
                    if (table_find(local_tab, token->attr.str, NULL)) {
                        // redefinition
                        terminate(DEFINITION_ERROR);
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
                        return;
                    }
                    else {
                        // assign '='

                        // creating token vector
                        get_token();   // necessary, will be included in vector
                        token_vector tv = read_to_semic();
                        // id ( )
                        if (tv->last >= 3 && check_if_func(tv->arr) ) {
                            // handling function
                            handle_function(tv->arr, tv->last, instr_list, sym,
                                            local_tab);
                        }
                        else if (tv->last > 0) {
                            // at least one value
                            part = 2;
                            precedence_analyser(tv->arr, tv->last, sym,
                                                local_tab, actual_class,
                                                instr_list );
                            part = 1;
                        }
                        else {
                            terminate(SYNTAX_ERROR);
                        }

                        token_vec_delete(tv);
                    }
                    return;
                }}}
            case TK_while:
            case TK_if:
                {{{
                    T_keyword keyword = token->attr.keyword;
                    // ST -> if ( EXPR ) { ST_LIST } ELSE
                    get_token();

                    // reading till `)`
                    // '(' is not included
                    T_instr *cond, *end_label;

                    // creating label - for while
                    cond = create_instr(instr_list, TI_lab, 0, 0, 0 );
                    end_label = instr_init(TI_lab, 0, 0, 0);

                    token_vector tv = read_to_rbrac();
                    // empty expression
                    if (tv->size == 1 ) {
                        terminate(SYNTAX_ERROR);
                    }
                    acc->attr.var->data_type = is_bool;
                    // result will be stored in accumulator
                    part = 2;
                    precedence_analyser(tv->arr, tv->last-1, acc, local_tab,
                                        actual_class, instr_list );
                    part = 1;
                    token_vec_delete(tv);

                    create_instr(instr_list, TI_jmpz, end_label, 0, 0);

                    get_token();    // '{'

                    if (token->type != TT_lCurlBracket) {
                        stat(local_tab, instr_list);
                    }
                    else {
                        st_list();
                    }

                    if (keyword == TK_while) {
                        // jump to condition
                        create_instr(instr_list, TI_jmp, cond, 0, 0);

                        // inserting end label
                        list_insert_last(instr_list, end_label);
                        return;
                    }
                    // if next word is else do call st_else()
                    get_token();

                    if (token->type == TT_keyword &&
                        token->attr.keyword == TK_else)
                    {
                        // if condition satisfied jumping over else branch
                        T_instr *lab_out = instr_init(TI_lab,0,0,0);
                        create_instr(instr_list, TI_jmp, lab_out,0,0);
                        // if not go to else
                        list_insert_last(instr_list, end_label);
                        // insert else branh
                        st_else(local_tab, instr_list);
                        // label under else branch
                        list_insert_last(instr_list, lab_out);
                    }
                    else {
                        list_insert_last(instr_list, end_label);
                        // no else read, must unget
                        unget_token();
                    }

                    return;
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
                        terminate(4);
                    }
                    // return value will be stored in accumulator
                    acc->attr.var->data_type = dtype;
                    part = 2;
                    if (dtype != is_void)
                        precedence_analyser(tv->arr, tv->last, acc, local_tab,
                                            actual_class, instr_list );
                    part = 1;
                    token_vec_delete(tv);
                    create_instr(instr_list, TI_ret, 0, 0, 0);
                    return;
                }}}
            default:
                terminate(SYNTAX_ERROR);
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
            terminate(SYNTAX_ERROR);
        // token iterator
        T_token *it = tv->arr;

        char *iden = it->attr.str;       // identifier name
        T_symbol *loc_sym, *glob_sym;    // local and global symbol

        // checking whether it was defined
        loc_sym = table_find_simple(local_tab, iden, NULL);    // local var
        glob_sym = table_find(symbol_tab, iden, actual_class); // static

        // undefined reference
        if (!loc_sym && !glob_sym ) {
           terminate(DEFINITION_ERROR);
        }
        // getting '=' or '('
        it++;
        if (it->type == TT_assign)
        {{{
            // id = ....;
            T_symbol *sym = loc_sym ? loc_sym : glob_sym;
            // assignment goes to function or class
            if (!sym || sym->symbol_type != is_var) {
                terminate(DEFINITION_ERROR);
            }
            // go after expression or function
            it++;
            // id = id ( )
            if (tv->last >= 5 && check_if_func(it) ) {
                // token count
                unsigned tcount = tv->last - 2;
                handle_function(it, tcount, instr_list, sym, local_tab);
            }
            else if (tv->last >= 3) {
                part = 2;
                precedence_analyser(it, tv->last - 2, sym, local_tab,
                                    actual_class, instr_list );
                part = 1;
            }
            else {
                terminate(SYNTAX_ERROR);
            }
            token_vec_delete(tv);
            return;
        }}}
        else
        {{{
            handle_function(tv->arr, tv->last, instr_list, NULL, local_tab);
            token_vec_delete(tv);
            return;
        }}}
    }}}
}}}

// ELSE -> .
// ELSE -> else ELSE2
// ELSE2 -> { ST_LIST }
static void st_else(T_symbol_table *local_tab, ilist *instr_list)
{{{
    fun = __func__;

    get_token();
    // `else {`
    if (token->type == TT_lCurlBracket) {
        st_list();
    }
    // `else if`
    else if (token->attr.keyword == TK_if) {
        st_else2(local_tab, instr_list);
    }
    else {
        stat(local_tab, instr_list);
    }
}}}

// ELSE2 -> if ( EXPR ) { ST_LIST } ELSE
static void st_else2(T_symbol_table *local_tab, ilist *instr_list)
{{{
    fun = __func__;

    // '(' read
    get_token();
    // reading till ')'
    token_vector tv = read_to_rbrac();
    if (tv->size == 1 ) {
        terminate(SYNTAX_ERROR);
    }

    // result will be stored in accumulator
    part = 2;
    precedence_analyser(tv->arr, tv->last-1, acc, local_tab, actual_class,
                        instr_list );
    part = 1;

    token_vec_delete(tv);

    T_instr *end_lab = instr_init(TI_lab, 0, 0, 0);
    create_instr(instr_list, TI_jmpz, end_lab, 0, 0);
    // {
    get_token();
    // beginning of statement list
    if (token->type == TT_lCurlBracket)
        st_list();
    else
        stat(local_tab, instr_list);

    // if next word is else do call st_else()
    get_token();

    if (token->type == TT_keyword && token->attr.keyword == TK_else) {
        T_instr *lab_out = instr_init(TI_lab, 0, 0, 0);
        create_instr(instr_list, TI_jmp, lab_out, 0, 0);
        list_insert_last(instr_list, end_lab);
        st_else(local_tab, instr_list);
        list_insert_last(instr_list, lab_out);
    }
    else {
        list_insert_last(instr_list, end_lab);
        // no else, token must be returned
        unget_token();
    }
}}}

int second_throughpass() {
    part = 1;
    row = 1;
    acc = table_find_simple(symbol_tab, "|accumulator|",NULL);
    prog();
    //print_table(symbol_tab);
    return 0;
}

