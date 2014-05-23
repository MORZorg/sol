#include <stdlib.h>

#ifndef __STACKLIST_H__
#define __STACKLIST_H__

#define STACK_OK 0
#define STACK_ERROR -1

typedef void* stack_t;
typedef struct entry* stacklist;

typedef struct entry
{
	stack_t table;
	struct entry* next;
} Entry;

stacklist new_stack();
Entry* new_entry( stack_t );
int stacklist_push( stacklist*, stack_t );
int stacklist_pop( stacklist* );

#endif // __STACKLIST_H__
