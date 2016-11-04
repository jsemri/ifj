#include "instruction.h"

// Jeste je potreba pridat parametry funkce (pocet a po rade typ)	!!!
typedef struct htab_item_s {
	struct htab_item_s *next;
    const char* id; // key
    typedef enum { // enum type {var, func, class}
    	s_var,
        s_int,
        s_class,
    } T_type;
    typedef enum { // {T_string, double, int}
    	T_string,
        T_double,
        T_int,
    } T_data_type;
    typedef union {
        double d;
        int n;
        T_string *s;
    } T_value;
    typedef union {
        bool is_init; // initialization of variable
        instr_li * ilist; // body of function
    } T_other;
    struct htab_item_s *p_class; 
    bool is_defined; // flag
} htab_item;

typedef struct htab_s{
	unsigned htab_size;
	struct htab_item_s *list[];
} htab;















/*
typedef struct htab_listitem_s{
	struct htab_listitem_s *next;
	unsigned data;
	char key[];
} htab_listitem;

typedef struct htab{
	unsigned htab_size;
	struct htab_listitem_s *list[];
} htab_t;
*/




htab* htab_init(unsigned size);

unsigned int hash_function(const char *str, unsigned htab_size);

void htab_free(htab *t);

void htab_clear(htab *t);

void htab_remove(htab *t, const char *key);

void htab_foreach(htab *t, void (*function)(char key[], unsigned value));

htab_item* htab_create(const char *key);

htab_item* htab_lookup(htab *t, const char *key);


