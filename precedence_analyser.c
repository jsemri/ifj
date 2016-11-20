/**
 * Precedence analyser
 */

#include "precedence_analyser.h"
#include "precedence_analyser_stack.h"
#include "precedence_table.h"
#include "precedence_rules.h"
#include "globals.h"
#include "symbol.h"
#include <stdio.h>

#define MAX_TERMS_IN_RULE 3

#define PUSH_SYMBOL(s) prec_stack_push_symbol(stack, s)
#define PUSH_TOKEN(t) prec_stack_push_token(stack, t)
#define PUSH_EXP(t) prec_stack_push_exp(stack, t)
#define ADD_HANDLE() prec_stack_add_handle(stack)

static T_token* get_next_token(T_token *cur_token, int token_count);

/*
 * TODO Document me!
 */
void precedence_analyser(T_token *first_token, int token_count,
                         T_symbol *lvalue, T_symbol_table* local_table,
                         T_symbol *act_class, ilist *instr_list) {
    T_prec_stack *stack = prec_stack_new();
    PUSH_SYMBOL(PREC_TOP);

    T_token *token = first_token;
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
    }

    T_symbol *result = prec_stack_get_result(stack);
    if (lvalue != NULL) {
        // FIXME Přetypovat result na typ lvalue.
        result = convert(result, lvalue->attr.var->data_type, instr_list);
        create_instr(instr_list, TI_mov, result, NULL, lvalue);
        lvalue->attr.var->initialized = true;
        //printf("[INST] Ulož výsledek do lValue\n");
    }
    else {
        // TODO Ulož výsledek "někam" jinam
    }
    //prec_stack_print(stack);
    prec_stack_free();
}

/**
 * TODO Document me
 * @param
 * @return A pointer to next token to be processed
 */
static T_token* get_next_token(T_token *cur_token, int token_count) {
    static int read_tokens = 1;
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

int main() {
    token_vector v = token_vec_init();
    add_int(v, 10);
    add_token(v, TT_plus);
    add_int(v, 5);
    add_token(v, TT_plus);
    add_float(v, 3);
    add_token(v, TT_mul);
    add_int(v, 2);

    symbol_tab = table_init(RANGE);

    ilist *list = list_init();
    precedence_analyser(&v->arr[0], v->last, NULL, symbol_tab, NULL, list);

    token_vec_delete(v);
    return 0;
}