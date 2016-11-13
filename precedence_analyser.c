/**
 * Precedence analyser
 */

#include "precedence_analyser.h"
#include "precedence_analyser_stack.h"
#include "precedence_table.h"
#include "precedence_rules.h"
#include "token.h"
#include <stdio.h>

#define MAX_TERMS_IN_RULE 3

#define FAIL_PREC(code) do {prec_stack_free(stack); \
                            return code;} while(0)
                            /*destruct_rules();*/
#define PUSH_SYMBOL(s) if (!prec_stack_push_symbol(stack, s)) FAIL_PREC(99)
#define PUSH_TOKEN(t) if (!prec_stack_push_token(stack, t)) FAIL_PREC(99)
#define PUSH_EXP(t) if (!prec_stack_push_exp(stack, t)) FAIL_PREC(99)
#define ADD_HANDLE() if (!prec_stack_add_handle(stack)) FAIL_PREC(99)

int precedence_analyser(token_vector v) {
    T_prec_stack *stack = prec_stack_new();
    if (stack == NULL)
        return 99;
    PUSH_SYMBOL(PREC_TOP);
    T_token *token = get_next_token(v);

    if (!init_exp_table()) {
        prec_stack_free(stack);
        return 99;
    }

    // TODO Vypnout krokování :)
    int steps = 1, max = 99;
    while (token != NULL || !prec_stack_is_empty(stack)) {
        T_tokenType input_tt = token ? token->type : TT_eof;
        T_tokenType stack_tt = prec_stack_get_top_token_type(stack);
        T_precedence_state precedence = get_precedence(stack_tt, input_tt);
        if (precedence == PREC_LOWER) {
            //printf("Provést: <\n");
            ADD_HANDLE();
            PUSH_TOKEN(token);
            token = get_next_token(v);
        }
        else if (precedence == PREC_GREATER) {
            //printf("Provést: >\n");
            T_prec_stack_entry rule_terms[MAX_TERMS_IN_RULE];
            int count;
            if (!prec_stack_reduce(stack, rule_terms, MAX_TERMS_IN_RULE,
                                   &count)) {
                FAIL_PREC(2);
            }
            int errcode;
            T_symbol *result = execute_rule(rule_terms, count, &errcode);
            if (result == NULL)
                FAIL_PREC(errcode);
            PUSH_EXP(result);
        }
        else if (precedence == PREC_EQUAL) {
            //printf("Provést: =\n");
            PUSH_TOKEN(token);
            token = get_next_token(v);
        }
        else {
            //if (steps == max) printf("***\n");
            //fprintf(stderr, "Chybí operátor\n");
            FAIL_PREC(2);
        }
        if (steps++ == max)
            break;
    }
    prec_stack_print(stack);
    prec_stack_free(stack);
    return 0;
}

T_token* get_next_token(token_vector v) {
    static int read_tokens = 0;
    if (read_tokens == v->last) {
        return NULL;
    }
    return &v->arr[read_tokens++];
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

    int r = precedence_analyser(v);

    printf("%% Návratový kód: %d\n", r);

    token_vec_delete(v);
    return 0;
}