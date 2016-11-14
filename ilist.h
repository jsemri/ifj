#ifndef ILIST_H
#define ILIST_H

#include "instruction.h"

ilist *list_init();
void list_insert_first(ilist *L, T_instr *ins);
void list_insert_last(ilist *L, T_instr *ins);
void list_delete(ilist *L);
void list_free(ilist **L);
void list_first(ilist *L);
void list_succ(ilist *L);

#endif
