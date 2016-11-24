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
#include <stdlib.h>

// initialization of instruction list
ilist* list_init() {
    return calloc(1, sizeof(ilist));
}

// pushing instruction back
void list_insert_last(ilist *L, T_instr *ins) {

    if (L->last) {
        L->last->next = ins;
    }
    else {
        L->first = ins;
    }
    L->last = ins;
}

// deleting whole instruction list
void list_delete(ilist *L) {

    T_instr *ptr;
    while (L->first) {
        ptr = L->first;
        L->first = L->first->next;
        free(ptr);
    }
    L->last = NULL;
}

void list_free(ilist **L) {
    if (*L) {
        list_delete(*L);
        free(*L);
        *L = NULL;
    }
}

void list_merge(ilist *L1, ilist *L2) {
    if (L1->first)
        L1->last->next = L2->first;
    else
        L1->first = L2->first;
    L1->last = L2->last;
}

