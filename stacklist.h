#ifndef __STACKLIST_H__
#define __STACKLIST_H__

#define STACK_OK 0
#define STACK_ERROR -1

typedef void* any_t;
typedef any_t stacklist;

typedef struct entry
{
	any_t table;
	Entry* next;
} Entry;

stacklist new_stack();
Entry* new_entry( any_t );
int stacklist_push( stacklist, any_t );
int stacklist_pop( stacklist );

#endif // __STACKLIST_H__
