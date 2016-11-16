#include "instruction.h"
#include "ilist.h"
#include "globals.h"
#include <stdlib.h>

void create_instr(ilist *L, T_instr_type itype, void *op1, void *op2,
                  void *dest)
{

    T_instr *ins = calloc(1, sizeof(T_instr));

    if (!ins)
        terminate(INTERNAL_ERROR);

    ins->itype = itype;
    ins->op1 = op1;
    ins->op2 = op2;
    ins->dest = dest;
    list_insert_last(L, ins);
}


