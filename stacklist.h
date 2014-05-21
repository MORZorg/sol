#include <stdlib.h>

#ifndef __STACKLIST_H__
#define __STACKLIST_H__

#define STACK_OK 0
#define STACK_ERROR -1

typedef void* any_t;
typedef struct entry* stacklist;

typedef struct entry
{
	any_t table;
	struct entry* next;
} Entry;

stacklist new_stack();
Entry* new_entry( any_t );
int stacklist_push( stacklist*, any_t );
int stacklist_pop( stacklist* );

#endif // __STACKLIST_H__
