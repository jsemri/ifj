#include "instruction.h"
#include <stdlib.h>

// initialization of instruction list
ilist* list_init() {
    return calloc(1, sizeof(ilist));
}

// inserting instruction as first
void list_insert_first(ilist *L, T_instr *ins) {

    if (L->first) {
        ins->next = L->first;
    }
    else {
        L->last = ins;
    }
    L->first = ins;
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
    list_delete(*L);
    free(*L);
    *L = NULL;
}

