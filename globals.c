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

#include "globals.h"
#include "token.h"
#include "token_vector.h"
#include <stdio.h>
#include "ial.h"
#include "instruction.h"
#include "symbol.h"
#include "ilist.h"
#include "stack.h"
#include <stdlib.h>
#include <stdbool.h>
#include "debug.h"
#include "precedence_analyser_stack.h"

T_symbol_table *symbol_tab;
FILE *source;
T_token *token;
ilist *glist;
token_vector global_token_vector;
T_stack *frame_stack;
T_stack *main_stack;
char *char_vector;
T_instr *div_point;


void terminate(int err_code) {
    token_free(&token);
    if (global_token_vector)
        token_vec_delete(global_token_vector);
    prec_stack_free();
    stack_remove(&frame_stack, true);
    stack_remove(&main_stack, false);
    // dividing lists
    if (glist) {
        if (div_point) {
            div_point->next = NULL;
            glist->last = div_point;
        }
        else {
            glist->first = NULL;
            glist->last = NULL;
        }
    }

    list_free(&glist);
    fclose(source);
    table_remove(&symbol_tab);
    free(char_vector);

    exit(err_code);
}

