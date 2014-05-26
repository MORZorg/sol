#include "stacklist.h"

Entry* new_entry( stacklist_t table )
{
	Entry* result = malloc( sizeof( Entry ) );

	if( !result )
		return NULL; 

	result->table = table;
	result->next = NULL;

	return result;
}

int stacklist_push( stacklist* stack, stacklist_t table )
{
	Entry* result = new_entry( table );

	if( !result )
		return STACK_ERROR;

	result->next = *stack;
	*stack = result;

	return STACK_OK;
}

int stacklist_pop( stacklist* stack )
{
	Entry* delete_me = *stack;
	*stack = (*stack)->next;

	free( delete_me );

	return STACK_OK;
}

