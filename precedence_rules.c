/**
 * precedence_rules.c
 */

#include "precedence_analyser_stack.h"
#include "precedence_rules.h"
#include "globals.h"
#include <stdlib.h>

#define RULES_COUNT 15
T_prec_rule rules[RULES_COUNT] = {
    {3, TT_empty, TT_plus, TT_empty, rule_concat},
    {3, TT_empty, TT_minus, TT_empty, rule_arith},
    {3, TT_empty, TT_div, TT_empty, rule_arith},
    {3, TT_empty, TT_mul, TT_empty, rule_arith},
    {3, TT_empty, TT_equal, TT_empty, rule_bool},
    {3, TT_empty, TT_notEq, TT_empty, rule_bool},
    {3, TT_empty, TT_lesser, TT_empty, rule_bool},
    {3, TT_empty, TT_greater, TT_empty, rule_bool},
    {3, TT_empty, TT_lessEq, TT_empty, rule_bool},
    {3, TT_empty, TT_greatEq, TT_empty, rule_bool},
    {3, TT_lBracket, TT_empty, TT_rBracket, rule_brackets},
    {1, TT_id, TT_empty, TT_empty, rule_i_to_exp},
    {1, TT_int, TT_empty, TT_empty, rule_i_to_exp},
    {1, TT_double, TT_empty, TT_empty, rule_i_to_exp},
    {1, TT_string, TT_empty, TT_empty, rule_i_to_exp},
};

#define CONV_TERM_TO_TT(i) (terms[i].type == PREC_TOKEN ? \
                            terms[i].ptr.token->type : TT_empty)
#define FAIL_RULE(code) terminate(code)
#define CREATE_SYMBOL(symbol, type) T_symbol *symbol = create_var_uniq(type); \
                                    table_insert(symbol_tab, symbol); \
                                    symbol->attr.var->initialized = true
#define ADD_INSTR(type, dst, s1, s2) create_instr(instr_list, type, s1, s2, dst)
#define CHECK_TYPE(symbol, type) ((symbol)->attr.var->data_type == type)

T_symbol *execute_rule(T_prec_stack_entry terms[3], int count,
                       T_symbol_table *ltable, T_symbol *act_class,
                       ilist *instr_list) {
    for (int i = 0; i < RULES_COUNT; i++) {
        if (rules[i].tokens != count ||
                rules[i].tokens >= 1 && rules[i].t1 != CONV_TERM_TO_TT(0) ||
                rules[i].tokens >= 2 && rules[i].t2 != CONV_TERM_TO_TT(1) ||
                rules[i].tokens >= 3 && rules[i].t3 != CONV_TERM_TO_TT(2))
            continue;
        return rules[i].func(terms, ltable, act_class, instr_list);
    }
    // No rule can be applied:
    //printf("-- Nic nevyhovuje\n");
    FAIL_RULE(2);
    return NULL;
}

#define ALLOWED_CONVERSIONS_SIZE 6

/**
 * The list of valid conversions
 *
 * Each valid implicit converversion of the source data-type to the destination
 * data-type is represented here by two elements. The first element in the pair
 * represents the source, the second represents the target data-type.
 *
 * For example, ALLOWED_CONVERSIONS[] = {is_int, is_double, is_int, is_str}
 * means that an integer can be casted to double or to a String.
 *
 * All other conversions not listed here are prohibited and will lead
 * to TYPE_ERROR.
 */
T_data_type ALLOWED_CONVERSIONS[ALLOWED_CONVERSIONS_SIZE] = {
    is_int, is_double,
    is_int, is_str,
    is_double, is_str,
};

/**
 * Converts the input symbol to a new data type and adds the convertion
 * instruction to ilist.
 *
 * This function does check what the current data type of the input symbol is
 * and depending on the current data-type and the new date-type the function
 * will:
 *
 * a) Do nothing, if the input symbol is already in the target data type.
 *    The same T_symbol* that was inserted to the function in parameter 'in'
 *    will be returned.
 * b) Create a new T_symbol* and add a new instruction to the end of instruction
 *    list if the two data-type are different, but the source data-type can be
 *    casted to the target data-type.
 * c) Call terminate(TYPE_ERROR), if the input symbol can't be casted
 *    to new_type.
 *
 * @param in The input symbol
 * @param new_type The target data-type
 * @param ilist The instruction list (a new instuction will be added there
 *                   in case B).
 * @return The same, or a new, symbol with the target data-type
 */
T_symbol *convert(T_symbol *in, T_data_type new_type, ilist *instr_list) {
    if (CHECK_TYPE(in, new_type)) {
        // Both data types are already the same, do nothing
        return in;
    }

    bool is_allowed = false;
    for (int i = 0; i < ALLOWED_CONVERSIONS_SIZE; i += 2) {
        if (CHECK_TYPE(in, ALLOWED_CONVERSIONS[i])
                && ALLOWED_CONVERSIONS[i+1] == new_type) {
            is_allowed = true;
            break;
        }
    }
    if (!is_allowed) {
        terminate(TYPE_ERROR);
    }

    CREATE_SYMBOL(out, new_type);
    ADD_INSTR(TI_convert, out, in, NULL);
    //printf("[INST] Přetypování\n");

    return out;
}

static T_data_type cast_nums(T_symbol **s1, T_symbol **s2, ilist *instr_list) {
    if (CHECK_TYPE(*s1, is_int) && CHECK_TYPE(*s2, is_int))
        // Both operands are int, result will be int
        return is_int;
    *s1 = convert(*s1, is_double, instr_list);
    *s2 = convert(*s2, is_double, instr_list);
    return is_double;
}

T_symbol *rule_brackets(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list) {
    return terms[1].ptr.symbol;
}


T_symbol *rule_bool(T_prec_stack_entry terms[3],
                    T_symbol_table *ltable, T_symbol *act_class,
                    ilist *instr_list) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;
    cast_nums(&s1, &s2, instr_list);

    CREATE_SYMBOL(symbol, is_bool);

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

    //printf("[INST] Arithm. oper.\n");
    ADD_INSTR(type, symbol, s1, s2);

    return symbol;
}

T_symbol *rule_concat(T_prec_stack_entry terms[3],
                      T_symbol_table *ltable, T_symbol *act_class,
                      ilist *instr_list) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;

    if (!CHECK_TYPE(s1, is_str) && !CHECK_TYPE(s2, is_str))
        return rule_arith(terms, ltable, act_class, instr_list);

    s1 = convert(s1, is_str, instr_list);
    s2 = convert(s2, is_str, instr_list);

    //printf("[INST] Spojení řetězců\n");

    CREATE_SYMBOL(symbol, is_str);
    ADD_INSTR(TI_concat, symbol, s1, s2);

    return symbol;
}

T_symbol *rule_arith(T_prec_stack_entry terms[3],
                     T_symbol_table *ltable, T_symbol *act_class,
                     ilist *instr_list) {
    T_symbol *s1 = terms[0].ptr.symbol;
    T_symbol *s2 = terms[2].ptr.symbol;
    T_data_type out_type = cast_nums(&s1, &s2, instr_list);

    CREATE_SYMBOL(symbol, out_type);

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

    //printf("[INST] Arithm. oper.\n");
    ADD_INSTR(type, symbol, s1, s2);

    return symbol;
}


T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list) {
    if (terms[0].ptr.token->type == TT_id) {
        // TODO je i inicializovaná?
        return is_defined(terms[0].ptr.token->attr.str, ltable, act_class,
                          is_void);
    }

    if (terms[0].ptr.token->type == TT_int) {
        //printf("[INST] Nový symbol: int=%d\n", terms[0].ptr.token->attr.n);
        CREATE_SYMBOL(symbol, is_int);
        // TODO Přidat instrukci..
        return symbol;
    }
    else if (terms[0].ptr.token->type == TT_double) {
        //printf("[INST] Nový symbol: double=%g\n", terms[0].ptr.token->attr.d);
        CREATE_SYMBOL(symbol, is_double);
        // TODO Přidat instrukci..
        return symbol;
    }
    else {
        // Unexpected token
        FAIL_RULE(2);
    }
}
