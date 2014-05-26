#include <stdlib.h>

#ifndef __STACKLIST_H__
#define __STACKLIST_H__

#define STACK_OK 0
#define STACK_ERROR -1

typedef void* stacklist_t;
typedef struct entry* stacklist;

typedef struct entry
{
	stacklist_t table;
	struct entry* next;
} Entry;

Entry* new_entry( stacklist_t );
int stacklist_push( stacklist*, stacklist_t );
int stacklist_pop( stacklist* );

#endif // __STACKLIST_H__
