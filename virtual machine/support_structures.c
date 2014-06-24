/*
 * File: support_structures.c
 * Description: VM stacks and methods to access them
 * Date: 140624
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "support_structures.h"

int initialize_stacks()
{
	pc = ap = op = ip = 0;

	Adescr* first_activation = malloc( ASTACK_UNIT );
	astack = &first_activation;

	Odescr* first_object = malloc( OSTACK_UNIT );
	ostack = &first_object;

	istack = malloc( ISTACK_UNIT );
	
	asize = ASTACK_UNIT;
	osize = OSTACK_UNIT;
	isize = ISTACK_UNIT;

	return MEM_OK;
}

// Interaction with the istack
char top_istack()
{
	return istack[ ip - 1 ]; 
}

void pop_istack()
{
	free( &( istack[ --ip ] ) );
}

void push_istack( char value )
{
	if( ip == isize )
	{
		char* old_istack = istack;

		istack = malloc( sizeof( char ) * ( isize + ISTACK_UNIT ) );

		for( int i = 0; i < isize; i++ )
			istack[i] = old_istack[i];

		isize += ISTACK_UNIT;
		
		free( old_istack );
	}

	istack[ ip++ ] = value;
}

int pop_int()
{
	int value = (int) top_istack();

	pop_istack();

	return value;
}

void push_int( int value )
{
	push_istack( (char) value );
}

float pop_real()
{
	char object[ sizeof(float) ];

	for( int i = 0; i < sizeof(float); i++ )
	{
		object[i] = (char) top_istack();
		pop_istack();
	}

	float value;

	memcpy( &value, object, sizeof(value) );

	return value;
}

void push_real( float value )
{
	char object[ sizeof(float) ];

	memcpy( object, &value, sizeof( value ) );

	for( int i = 0; i < sizeof( float ); i++ )
		push_istack( object[i] );
}

char pop_char()
{
	char value = top_istack();

	pop_istack();

	return value;
}

void push_char( char value )
{
	push_istack( value );
}

char* pop_string( int size )
{
	char* value = malloc( sizeof(char) * size );

	for( int i = 0; i < size; i++ )
	{
		value[i] = top_istack();
		pop_istack();
	}

	return value;
}

void push_string( char* object )
{
	for( int i = 0; i < strlen( object ); i++ )
		push_istack( object[i] );
}

// Interactions with the other stacks
Odescr* top_ostack()
{
	return ostack[ op - 1 ];
}

void pop_ostack()
{
	free( &( ostack[ --op ] ) );
}

void push_ostack( Odescr* value )
{
	if( op == osize )
	{
		Odescr** old_ostack = ostack;

		ostack = malloc( sizeof( Odescr* ) * ( osize + OSTACK_UNIT ) );

		for( int i = 0; i < osize; i++ )
			ostack[i] = old_ostack[i];

		osize += OSTACK_UNIT;

		free( old_ostack );
	}

	ostack[ op++ ] = value;
}

Adescr* top_astack()
{
	return astack[ ap - 1 ];
}

void pop_astack()
{
	free( &( astack[ --ap ] ) );
}

void push_astack( Adescr* value )
{
	if( ap == asize )
	{
		Adescr** old_astack = astack;

		astack = malloc( sizeof( Adescr* ) * ( asize + ASTACK_UNIT ) );

		for( int i = 0; i < asize; i++ )
			astack[i] = old_astack[i];

		asize += ASTACK_UNIT;

		free( old_astack );
	}

	astack[ ap++ ] = value;
}
