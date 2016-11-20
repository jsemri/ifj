/**
 * precedence_rules.h
 */

#ifndef IFJ_PRECEDENCE_RULES_H
#define IFJ_PRECEDENCE_RULES_H

#include "token.h"
#include "ial.h"
#include "ilist.h"
#include <stdbool.h>

typedef struct {
    int tokens;
    T_tokenType t1;
    T_tokenType t2;
    T_tokenType t3;
    T_symbol* (*func)(T_prec_stack_entry[], T_symbol_table*, T_symbol*, ilist*);
} T_prec_rule;

T_symbol *execute_rule(T_prec_stack_entry terms[3], int count,
                       T_symbol_table *ltable, T_symbol *act_class,
                       ilist *instr_list);

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
T_symbol *convert(T_symbol *in, T_data_type new_type, ilist *instr_list);

bool init_exp_table();

T_symbol *rule_brackets(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list);

T_symbol *rule_bool(T_prec_stack_entry terms[3],
                    T_symbol_table *ltable, T_symbol *act_class,
                    ilist *instr_list);

T_symbol *rule_concat(T_prec_stack_entry terms[3],
                      T_symbol_table *ltable, T_symbol *act_class,
                      ilist *instr_list);

T_symbol *rule_arith(T_prec_stack_entry terms[3],
                     T_symbol_table *ltable, T_symbol *act_class,
                     ilist *instr_list);

T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list);

#endif //IFJ_PRECEDENCE_RULES_H
