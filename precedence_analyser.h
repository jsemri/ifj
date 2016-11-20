/**
 * precedence_analyser.h
 */

#ifndef IFJ_PRECEDENCE_ANALYSER_H
#define IFJ_PRECEDENCE_ANALYSER_H

#include "token.h"
#include "token_vector.h"
#include "instruction.h"
#include "symbol.h"
#include "ilist.h"
#include "ial.h"

void precedence_analyser(T_token *first_token, int token_count,
                         T_symbol *lvalue, T_symbol_table* local_table,
                         T_symbol *act_class, ilist *instr_list);

#endif //IFJ_PRECEDENCE_ANALYSER_H
