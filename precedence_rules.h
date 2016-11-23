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
 * precedence_rules.h
 */

#ifndef IFJ_PRECEDENCE_RULES_H
#define IFJ_PRECEDENCE_RULES_H

#include "token.h"
#include "ial.h"
#include "ilist.h"
#include "symbol.h"
#include "precedence_analyser_stack.h"
#include <stdbool.h>

typedef struct {
    int tokens;
    T_tokenType t1;
    T_tokenType t2;
    T_tokenType t3;
    T_symbol* (*func)(T_prec_stack_entry[], T_symbol_table*, T_symbol*, ilist*);
} T_prec_rule;


/**
 * Simulates a rule
 * @param terms The input terms.
 * @param count The number of valid input terms (max 3)
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this rule
 */
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

/**
 * Simalates rule: E -> (E)
 * @param terms The input terms.
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this expression
 */
T_symbol *rule_brackets(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list);

/**
 * Simalates rules: E -> E < E
 *                  E -> E <= E
 *                  E -> E > E
 *                  E -> E >= E
 *                  E -> E == E
 *                  E -> E != E
 * @param terms The input terms.
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this expression
 */
T_symbol *rule_bool(T_prec_stack_entry terms[3],
                    T_symbol_table *ltable, T_symbol *act_class,
                    ilist *instr_list);

/**
 * Simalates rule: E -> E + E
 *
 * The function supports both numeric addiction and String concatination.
 * (in case of numerical addiction, rule_arith will be called)
 *
 * @param terms The input terms.
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this expression
 */
T_symbol *rule_concat(T_prec_stack_entry terms[3],
                      T_symbol_table *ltable, T_symbol *act_class,
                      ilist *instr_list);

/**
 * Simalates rules: E -> E + E (addiction only, String concatination
 *                              is not supported)
 *                  E -> E - E
 *                  E -> E * E
 *                  E -> E / E
 * @param terms The input terms.
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this expression
 */
T_symbol *rule_arith(T_prec_stack_entry terms[3],
                     T_symbol_table *ltable, T_symbol *act_class,
                     ilist *instr_list);

/**
 * Simalates rule: E -> i
 * @param terms The input terms.
 * @param ltable The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated.
 * @return A symbol that represents the result of this expression
 */
T_symbol *rule_i_to_exp(T_prec_stack_entry terms[3],
                        T_symbol_table *ltable, T_symbol *act_class,
                        ilist *instr_list);

#endif //IFJ_PRECEDENCE_RULES_H
