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

#include "instruction.h"
#include "ilist.h"
#include "globals.h"
#include <stdlib.h>

void *create_instr(ilist *L, T_instr_type itype, void *op1, void *op2,
                  void *dest)
{{{

    T_instr *ins = calloc(1, sizeof(T_instr));

    if (!ins)
        terminate(INTERNAL_ERROR);

    ins->itype = itype;
    ins->op1 = op1;
    ins->op2 = op2;
    ins->dest = dest;
    list_insert_last(L, ins);

    return ins;
}}}

T_instr *instr_init(T_instr_type itype, void *op1, void *op2, void *dest)
{{{

    T_instr *ins = calloc(1, sizeof(T_instr));

    if (!ins)
        terminate(INTERNAL_ERROR);

    ins->itype = itype;
    ins->op1 = op1;
    ins->op2 = op2;
    ins->dest = dest;
    return ins;
}}}

