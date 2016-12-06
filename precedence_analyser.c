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

/**
 * Precedence analyser
 */

#include "precedence_analyser.h"
#include "precedence_analyser_stack.h"
#include "precedence_table.h"
#include "precedence_rules.h"
#include "globals.h"
#include "symbol.h"
#include "token.h"
#include <stdio.h>
#include <assert.h>

#define MAX_TERMS_IN_RULE 3

#define PUSH_SYMBOL(s) prec_stack_push_symbol(stack, s)
#define PUSH_TOKEN(t) prec_stack_push_token(stack, t)
#define PUSH_EXP(t) prec_stack_push_exp(stack, t)
#define ADD_HANDLE() prec_stack_add_handle(stack)

static T_token* get_next_token(T_token *cur_token, int token_count);
int read_tokens;

/**
 * The entry point of the precedence analyser
 *
 * This analyser will generate instuctions, which will calculate the result
 * of the input expression. The result of the expression will be in 'lvalue'.
 *
 * For example, for expression 1 + 2 * 3 these instruction will be added
 * to the instruction list 'instr_list':
 *
 * MUL [tmp1], 2, 3
 * ADD [tmp2], 1, [tmp1]
 * MOV [lvalue], [tmp2]
 *
 * @param first_token The first token of the expression
 * @param token_count Number of tokens of the expression
 * @param lvalue Symbol where the result of the expression will be stored
 * @param local_table The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instruction list where the instruction will be
 *                   generated. The new instruction will be added to the end
 *                   of the list.
 */
void precedence_analyser(T_token *first_token, int token_count,
                         T_symbol *lvalue, T_symbol_table* local_table,
                         T_symbol *act_class, ilist *instr_list) {
    assert(first_token != NULL);
    assert(token_count >= 0);
    assert(lvalue != NULL);
    assert(instr_list != NULL);


    T_prec_stack *stack = prec_stack_new();
    PUSH_SYMBOL(PREC_TOP);

    T_token *token = first_token;
    read_tokens = 1;
    while (token != NULL || !prec_stack_is_empty(stack)) {
        T_tokenType input_tt = token ? token->type : TT_eof;
        T_tokenType stack_tt = prec_stack_get_top_token_type(stack);
        T_precedence_state precedence = get_precedence(stack_tt, input_tt);
        if (precedence == PREC_LOWER) {
            //printf("Provést: <\n");
            ADD_HANDLE();
            PUSH_TOKEN(token);
            token = get_next_token(token, token_count);
        }
        else if (precedence == PREC_GREATER) {
            //printf("Provést: >\n");
            T_prec_stack_entry rule_terms[MAX_TERMS_IN_RULE];
            int count;
            prec_stack_reduce(stack, rule_terms, MAX_TERMS_IN_RULE, &count);
            T_symbol *result = execute_rule(rule_terms, count, local_table,
                                            act_class, instr_list);
            PUSH_EXP(result);
        }
        else if (precedence == PREC_EQUAL) {
            //printf("Provést: =\n");
            PUSH_TOKEN(token);
            token = get_next_token(token, token_count);
        }
        else {
            //fprintf(stderr, "Chybí operátor\n");
            terminate(SYNTAX_ERROR);
        }
        //prec_stack_print(stack);
        //printf("---------------\n");
    }

    T_symbol *result = prec_stack_get_result(stack);
    //printf("[INST] Ulož výsledek z [%s] do [%s]\n", result->id, lvalue->id);
    result = convert(result, lvalue->attr.var->data_type, instr_list);
    create_instr(instr_list, TI_mov, result, NULL, lvalue);

    //prec_stack_print(stack);
    prec_stack_free();
}

/**
 * Reads the next token and returns it.
 *
 * If all tokens from the expression were read, NULL will be returned.
 *
 * @param cur_token The token that is currently processed
 * @param token_count Number of token that should be read (in total)
 * @return A pointer to next token to be processed
 */
static T_token* get_next_token(T_token *cur_token, int token_count) {
    if (read_tokens == token_count) {
        return NULL;
    }
    read_tokens++;
    return cur_token + 1;
}

/*
 * A few tests...
 *
 * TODO Move elsewhere
 */
/*
void add_token(token_vector v, T_tokenType type) {
    T_token *token = token_new();
    token->type = type;
    token_push_back(v, token);
}
void add_int(token_vector v, int i) {
    T_token *token = token_new();
    token->type = TT_int;
    token->attr.n = i;
    token_push_back(v, token);
}
void add_float(token_vector v, double i) {
    T_token *token = token_new();
    token->type = TT_double;
    token->attr.d = i;
    token_push_back(v, token);
}

/ *int main0() {
    token_vector v = token_vec_init();
    / *add_int(v, 10);
    add_token(v, TT_plus);
    add_token(v, TT_lBracket);
    add_int(v, 5);
    add_token(v, TT_plus);
    add_float(v, 3);
    add_token(v, TT_mul);
    add_int(v, 2);
    add_token(v, TT_rBracket);* /
    //add_float(v, 6);
    //add_token(v, TT_greater);
    //add_int(v, 4);

    symbol_tab = table_init(RANGE);
    T_token *token = token_new();
    token->type = TT_string;
    token->attr.str = "Ahoj";
    token_push_back(v, token);
    add_token(v, TT_plus);
    add_int(v, 3);

    T_symbol *lval = create_var_uniq(is_str);

    ilist *list = list_init();
    precedence_analyser(&v->arr[0], v->last, lval, symbol_tab, NULL, list);

    token_vec_delete(v);
    return 0;
}*/