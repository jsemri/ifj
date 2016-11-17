/**
 * precedence_rules.h
 */

#ifndef IFJ_PRECEDENCE_RULES_H
#define IFJ_PRECEDENCE_RULES_H

#include "token.h"
#include "ial.h"
#include <stdbool.h>

typedef struct {
    int tokens;
    T_tokenType t1;
    T_tokenType t2;
    T_tokenType t3;
    T_symbol* (*func)(T_prec_stack_entry[], int*, T_func_symbol*, T_symbol*,
                      ilist*);
} T_prec_rule;

T_symbol *execute_rule(T_prec_stack_entry terms[3], int count, int *errcode,
                       T_func_symbol *act_func, T_symbol *act_class,
                       ilist *expr_ilist);

bool init_exp_table();

T_symbol *rule_concat(T_prec_stack_entry terms[3], int *errcode,
                      T_func_symbol *act_func, T_symbol *act_class,
                      ilist *expr_ilist);

T_symbol *rule_arith(T_prec_stack_entry terms[3], int *errcode,
                     T_func_symbol *act_func, T_symbol *act_class,
                     ilist *expr_ilist);

T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3], int *errcode,
                        T_func_symbol *act_func, T_symbol *act_class,
                        ilist *expr_ilist);

#endif //IFJ_PRECEDENCE_RULES_H
