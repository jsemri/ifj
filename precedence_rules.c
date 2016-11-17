/**
 * precedence_rules.c
 */

#include "precedence_analyser_stack.h"
#include "precedence_rules.h"
#include "token_vector.h"
#include "token.h"
#include "ial.h"
#include "globals.h"
#include "symbol.h"
#include "ilist.h"
#include <stdio.h>
#include <stdlib.h>

#define RULES_COUNT 14
T_prec_rule rules[RULES_COUNT] = {
    (T_prec_rule) {3, TT_empty, TT_plus, TT_empty, rule_concat},
    (T_prec_rule) {3, TT_empty, TT_minus, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_div, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_mul, TT_empty, rule_arith},
    (T_prec_rule) {3, TT_empty, TT_equal, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_empty, TT_notEq, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_empty, TT_lesser, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_empty, TT_greater, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_empty, TT_lessEq, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_empty, TT_greatEq, TT_empty, rule_bool},
    (T_prec_rule) {3, TT_lBracket, TT_empty, TT_rBracket, rule_brackets},
    (T_prec_rule) {1, TT_id, TT_empty, TT_empty, rule_i_to_exp},
    (T_prec_rule) {1, TT_int, TT_empty, TT_empty, rule_i_to_exp},
    (T_prec_rule) {1, TT_double, TT_empty, TT_empty, rule_i_to_exp},
};

#define CONV_TERM_TO_TT(i) (terms[i].type == PREC_TOKEN ? \
                            terms[i].ptr.token->type : TT_empty)
#define FAIL_RULE(code) do{*errcode = code; return NULL;} while (0)

#define ADD_INSTR(type, dst, s1, s2) create_instr(expr_ilist, type, s1, s2, dst)
#define CHECK_TYPE(symbol, type) ((symbol)->attr.var->data_type == type)

T_symbol *execute_rule(T_prec_stack_entry terms[3], int count, int *errcode,
                       T_func_symbol *act_func, T_symbol *act_class,
                       ilist *expr_ilist) {
    for (int i = 0; i < RULES_COUNT; i++) {
        if (rules[i].tokens != count ||
                rules[i].tokens >= 1 && rules[i].t1 != CONV_TERM_TO_TT(0) ||
                rules[i].tokens >= 2 && rules[i].t2 != CONV_TERM_TO_TT(1) ||
                rules[i].tokens >= 3 && rules[i].t3 != CONV_TERM_TO_TT(2))
            continue;
        return rules[i].func(terms, errcode, act_func, act_class, expr_ilist);
    }
    // No rule can be applied:
    //printf("-- Nic nevyhovuje\n");
    FAIL_RULE(2);
}

static T_symbol *conv(T_symbol *in, T_data_type new_type, ilist *expr_ilist) {
    // TODO Udělat všude
    T_symbol *out = create_var_uniq(new_type);

    ADD_INSTR(TI_convert, out, in, NULL);
    printf("[INST] Přetypování\n");

    return out;
}

static T_data_type cast_nums(T_symbol **s1, T_symbol **s2, ilist *expr_ilist) {
    if (CHECK_TYPE(*s1, is_int) && CHECK_TYPE(*s2, is_int))
        // Both operands are int, result will be int
        return is_int;
    else if (CHECK_TYPE(*s1, is_double) && CHECK_TYPE(*s2, is_double))
        // Both operands are double, result will be double
        return is_double;
    else if (CHECK_TYPE(*s1, is_int) && CHECK_TYPE(*s2, is_double)) {
        // First operand need to be casted to double
        *s1 = conv(*s1, is_double, expr_ilist);
        return is_double;
    }
    else if (CHECK_TYPE(*s1, is_double) && CHECK_TYPE(*s2, is_int)) {
        // Second operand need to be casted to double
        *s2 = conv(*s2, is_double, expr_ilist);
        return is_double;
    }
    else {
        // Input numbers can't be converted to a single numeric type.
        return is_void;
    }
}

T_symbol *rule_brackets(T_prec_stack_entry terms[3], int *errcode,
                        T_func_symbol *act_func, T_symbol *act_class,
                        ilist *expr_ilist) {
    return terms[1].ptr.symbol;
}


T_symbol *rule_bool(T_prec_stack_entry terms[3], int *errcode,
                    T_func_symbol *act_func, T_symbol *act_class,
                    ilist *expr_ilist) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;
    T_data_type out_type = cast_nums(&s1, &s2, expr_ilist);
    if (out_type == is_void) {
        // Input operands are not numbers
        FAIL_RULE(4);
    }

    T_symbol *symbol = create_var_uniq(is_bool);

    T_instr_type type;
    if (terms[1].ptr.token->type == TT_equal) {
        type = TI_equal;
    } else if (terms[1].ptr.token->type == TT_notEq) {
        type = TI_notequal;
    } else if (terms[1].ptr.token->type == TT_lesser) {
        type = TI_less;
    } else if (terms[1].ptr.token->type == TT_greater) {
        type = TI_greater;
    } else if (terms[1].ptr.token->type == TT_lessEq) {
        type = TI_lessEq;
    } else {
        type = TI_greaterEq;
    }

    printf("[INST] Arithm. oper.\n");
    ADD_INSTR(type, symbol, s1, s2);

    return symbol;
}

T_symbol *rule_concat(T_prec_stack_entry terms[3], int *errcode,
                      T_func_symbol *act_func, T_symbol *act_class,
                      ilist *expr_ilist) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;

    if (!CHECK_TYPE(s1, is_str) && !CHECK_TYPE(s2, is_str))
        return rule_arith(terms, errcode, act_func, act_class, expr_ilist);

    if (CHECK_TYPE(s1, is_int) || CHECK_TYPE(s1, is_double))
        s1 = conv(s1, is_str, expr_ilist);
    if (CHECK_TYPE(s2, is_int) || CHECK_TYPE(s2, is_double))
        s2 = conv(s2, is_str, expr_ilist);

    if (!CHECK_TYPE(s1, is_str) || !CHECK_TYPE(s2, is_str))
        // Operands can't be converted to String
        FAIL_RULE(4);

    T_symbol *out = create_var_uniq(is_str);
    printf("[INST] Spojení řetězců\n");
    ADD_INSTR(TI_concat, out, s1, s2);

    return out;
}

T_symbol *rule_arith(T_prec_stack_entry terms[3], int *errcode,
                     T_func_symbol *act_func, T_symbol *act_class,
                     ilist *expr_ilist) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;
    T_data_type out_type = cast_nums(&s1, &s2, expr_ilist);
    if (out_type == is_void) {
        // Input operands are not numbers
        FAIL_RULE(4);
    }

    T_symbol *symbol = create_var_uniq(out_type);

    T_instr_type type;
    if (terms[1].ptr.token->type == TT_plus) {
        type = TI_add;
    } else if (terms[1].ptr.token->type == TT_minus) {
        type = TI_sub;
    } else if (terms[1].ptr.token->type == TT_mul) {
        type = TI_mul;
    } else {
        type = TI_div;
    }

    printf("[INST] Arithm. oper.\n");
    ADD_INSTR(type, symbol, s1, s2);

    return symbol;
}


T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3], int *errcode,
                        T_func_symbol *act_func, T_symbol *act_class,
                        ilist *expr_ilist) {
    if (terms[0].ptr.token->type == TT_id) {
        // TODO je i inicializovaná?
        return is_defined(terms[0].ptr.token->attr.str, act_func->local_table,
                          act_class, is_void);
    }

    if (terms[0].ptr.token->type == TT_int) {
        // TODO Přidat instrukci..
        printf("[INST] Nový symbol: int=%d\n", terms[0].ptr.token->attr.n);
        return create_var_uniq(is_int);
    }
    else if (terms[0].ptr.token->type == TT_double) {
        // TODO Přidat instrukci..
        printf("[INST] Nový symbol: double=%g\n", terms[0].ptr.token->attr.d);
        return create_var_uniq(is_double);
    }
    else {
        // Unexpected token
        FAIL_RULE(2);
    }
}
