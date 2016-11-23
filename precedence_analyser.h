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

/**
 * The entry point of the precedence analyser
 *
 * This analyser will generate instuctions, which will calculate the result
 * of the input expression. The result of the expression will be in 'lvalue'.
 *
 * For example, for expression 1 + 2 * 3 these instruction will be added
 * to the instruction list 'instr_list':
 *
 * MUL [tmp1], 2, 3
 * ADD [tmp2], 1, [tmp1]
 * MOV [lvalue], [tmp2]
 *
 * @param first_token The first token of the expression
 * @param token_count Number of tokens of the expression
 * @param lvalue Symbol where the result of the expression will be stored
 * @param local_table The table with local variables
 * @param act_class The current class, if any.
 * @param instr_list The instrunction list where the instruction will be
 *                   generated. The new instruction will be added to the end
 *                   of the list.
 */
void precedence_analyser(T_token *first_token, int token_count,
                         T_symbol *lvalue, T_symbol_table* local_table,
                         T_symbol *act_class, ilist *instr_list);

#endif //IFJ_PRECEDENCE_ANALYSER_H
