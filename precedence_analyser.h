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


void precedence_analyser(token_vector v, T_symbol *lvalue,
                         T_func_symbol *act_func, T_symbol *act_class,
                         struct T_ilist *ilist);

#endif //IFJ_PRECEDENCE_ANALYSER_H
