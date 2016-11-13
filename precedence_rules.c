/**
 * precedence_rules.c
 */

#include "precedence_analyser_stack.h"
#include "precedence_rules.h"
#include "token_vector.h"
#include "token.h"
#include "ial.h"
#include <stdio.h>
#include <stdlib.h>

#define RULES_COUNT 12
T_prec_rule rules[RULES_COUNT] = {
    (T_prec_rule) {3, TT_empty, TT_plus, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_minus, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_div, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_mul, TT_empty, rule_arith},
    //(T_prec_rule) {3, TT_empty, TT_equal, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_empty, TT_notEq, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_empty, TT_lesser, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_empty, TT_greater, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_empty, TT_lessEq, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_empty, TT_greatEq, TT_empty, rule_add},
    //(T_prec_rule) {3, TT_lBracket, TT_empty, TT_rBracket, rule_add},
    (T_prec_rule) {1, TT_int, TT_empty, TT_empty, rule_i_to_exp},
    (T_prec_rule) {1, TT_double, TT_empty, TT_empty, rule_i_to_exp},
};

#define CONV_TERM_TO_TT(i) (terms[i].type == PREC_TOKEN ? \
                            terms[i].ptr.token->type : TT_empty)
#define FAIL_RULE(code) do{*errcode = code; return NULL;} while (0)

#define MAKE_NEW_SYMBOL(symbol, type) \
                T_symbol *symbol; do { \
                    symbol = malloc(sizeof(T_symbol)); \
                    if (symbol == NULL) FAIL_RULE(99); \
                    symbol->data_type = type; symbol->id = ""; \
                    T_var_symbol *v = malloc(sizeof(T_var_symbol)); \
                    if (v == NULL) { free(symbol); FAIL_RULE(99); } \
                    symbol->attr.var = v; \
                    table_insert(expressions, symbol); \
                } while (0)

T_symbol_table *expressions;

bool init_exp_table() {
    expressions = table_init(1);
    return expressions != NULL;
}

// TODO delete exp table!

T_symbol *execute_rule(T_prec_stack_entry terms[3], int count, int *errcode) {
    for (int i = 0; i < RULES_COUNT; i++) {
        if (rules[i].tokens != count ||
                rules[i].tokens >= 1 && rules[i].t1 != CONV_TERM_TO_TT(0) ||
                rules[i].tokens >= 2 && rules[i].t2 != CONV_TERM_TO_TT(1) ||
                rules[i].tokens >= 3 && rules[i].t3 != CONV_TERM_TO_TT(2))
            continue;
        return rules[i].func(terms, errcode);
    }
    // No rule can be applied:
    printf("-- Nic nevyhovuje\n");
    FAIL_RULE(2);
}

static T_symbol *conv_int_to_double(T_symbol *in, int *errcode) {
    MAKE_NEW_SYMBOL(out, is_double);

    out->attr.var->value.d = (double) in->attr.var->value.num;
    printf("[INST] Přetypování int->float\n");
    return out;
}

T_symbol *rule_arith(T_prec_stack_entry terms[3], int *errcode) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;
    T_data_types out_type;
    if (s1->data_type == is_int && s2->data_type == is_int)
        // Both operands are int, result will be int
        out_type = is_int;
    else if (s1->data_type == is_double && s2->data_type == is_double)
        // Both operands are double, result will be double
        out_type = is_double;
    else if (s1->data_type == is_int && s2->data_type == is_double) {
        // First operand need to be casted to double
        s1 = conv_int_to_double(s1, errcode);
        if (s1 == NULL)
            FAIL_RULE(99);
        out_type = is_double;
    }
    else if (s1->data_type == is_double && s2->data_type == is_int) {
        // Second operand need to be casted to double
        s2 = conv_int_to_double(s2, errcode);
        if (s2 == NULL)
            FAIL_RULE(99);
        out_type = is_double;
    }
    else {
        // Input numbers can't be converted to a single type.
        FAIL_RULE(4);
    }

    MAKE_NEW_SYMBOL(symbol, out_type);
    printf("[INST] Arithm. oper.\n");
    return symbol;
}

T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3], int *errcode) {

    // TODO Co, když "i" bude přímo symbol v tabulce?

    MAKE_NEW_SYMBOL(symbol, is_void);

    if (terms[0].ptr.token->type == TT_int) {
        symbol->attr.var->value.num = terms[0].ptr.token->attr.n;
        symbol->data_type = is_int;
    }
    else if (terms[0].ptr.token->type == TT_double){
        symbol->attr.var->value.d = terms[0].ptr.token->attr.d;
        symbol->data_type = is_double;
    }

    return symbol;
}
