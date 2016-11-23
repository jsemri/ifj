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
void list_merge(ilist *L1, ilist *L2);

#endif
