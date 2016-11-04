#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"



// Function init for creating and initialization of hash table
htab* htab_init(unsigned size)
{
    htab* table = malloc(sizeof(htab) + size*sizeof(htab_item));
    if(table == NULL)
        return NULL;

    table->htab_size = size;

    for(unsigned i = 0; i < size; i++)
    {
        table->list[i] = NULL;
    }

    return table;
}


// Function create - creates an item
htab_item* htab_create(const char *key, T_type c_type,
                       T_data_type c_data_type, T_value c_value, T_other c_other,
                       struct htab_item_s *c_p_class, bool c_is_defined)
{

    htab_item* item;

    item = malloc(sizeof(htab_item) + sizeof(char) * (strlen(key)+1));
    if(item == NULL)
    {
        return NULL;
    }

    memcpy(item->id, key, strlen(key)+1);
    item->next = NULL;
    item->T_type = c_type;
    item->T_data_type = c_data_type;
    item->T_value = c_value;
    item->T_other = c_other;
    item->p_class = c_p_class;
    item->is_defined = c_is_defined;


    return item;
}


// Function hash_function - determines an index to hash table
unsigned int hash_function(const char *str, unsigned htab_size) {
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}


// Function htab_lookup - looks up an item, returns pointer to the item if found, if not returns NULL
htab_item* htab_lookup(htab *t, const char *key)
{

    unsigned index = hash_function(key, t->htab_size);
    htab_item* item = t->list[index];
    htab_item* item_ptr = NULL;

    if(item == NULL)
    {
        return NULL;

    }else{
        while(item != NULL)
        {
            if(strcmp(item->id,key)==0)
            {
                item_ptr = item;
                break;
            }else{
                item_ptr = item;
                item = item->next;
            }
        }

        if(item_ptr != NULL && item != item_ptr)
        {
            return NULL;

        }
    }

    return item;


}


// Function htab_free - annulment of a hash_table (calls function clear)
void htab_free(htab *t){
    if(t != NULL)
    {
        htab_clear(t);
        free(t);
    }
}

// Function clear - clears all the items in the hash table
void htab_clear(htab *t)
{
    htab_item* item = NULL;

    for(unsigned i = 0; i < t->htab_size; i++)
    {
        while((item = t->list[i]) != NULL)
        {
            htab_remove(t, item->id);
        }
    }
}


// Function htab_remove - finds and removes certain item
void htab_remove(htab *t, const char *key)
{
    unsigned index = hash_function(key, t->htab_size);
    htab_item* item = t->list[index];
    htab_item* item_ptr = t->list[index];


    while(item != NULL)
    {
        if(strcmp(item->id,key) == 0)
        {
            if(item_ptr == item)
            {
                t->list[index] = item->next;
            }
            item_ptr->next = item->next;

            free(item);

            break;

        }else{
            item_ptr = item;
            item = item->next;
        }
    }
}

/* PŘÍPADNĚ, POKUD BUDE POTŘEBA
// Funkce foreach volajici funkci na vsechny prvky pole
void htab_foreach(htab *t, void (*function)(char key[], unsigned value))
{
	htab_item* item = NULL;

	for(unsigned i = 0; i < t->htab_size; i++)
	{
		item = t->list[i];
		while(item != NULL)
		{
			function(item->key, item->data);
			item = item->next;
		}
	}
}

*/



