#include "stacklist.h"

stacklist new_stack()
{
	stacklist result = malloc( sizeof( Entry ) );

	return result;
}

Entry* new_entry( stack_t table )
{
	Entry* result = malloc( sizeof( Entry ) );

	if( !result )
		return NULL; 

	result->table = table;
	result->next = NULL;

	return result;
}

int stacklist_push( stacklist* stack, stack_t table )
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

