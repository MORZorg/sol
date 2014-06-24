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
byte top_istack()
{
	return istack[ ip - 1 ]; 
}

void pop_istack()
{
	free( &( istack[ --ip ] ) );
}

void push_istack( byte value )
{
	if( ip == isize )
	{
		byte* old_istack = istack;

		istack = malloc( sizeof( byte ) * ( isize + ISTACK_UNIT ) );

		int i;
		for( i = 0; i < isize; i++ )
			istack[i] = old_istack[i];

		isize += ISTACK_UNIT;
		
		free( old_istack );
	}

	istack[ ip++ ] = value;
}

byte* pop_bytearray( int size )
{
	int i = size - 1;
	byte* value;

	do
	{
		value[ i ] = top_istack();

		pop_istack();
	}
	while( --i >= 0 );

	return value;
}

void push_bytearray( byte* value, int size )
{
	int i = 0;

	do
	{
		push_istack( value[ i ] );
	}
	while( ++i < size );
}

int pop_int()
{
	byte* object = pop_bytearray( sizeof( int ) );

	int i = 0;
	int value = 0;

	do
	{
		value += (int) ( ( object[ i ] << ( i * 8 ) ) & 0xFF );
	}
	while( ++i < sizeof( int ) );

	return value;
}

void push_int( int value )
{
	byte object[ sizeof( int ) ];

	int i = 0;

	do
	{
		object[ i ] = ( value >> ( i * 8 ) ) & 0xFF;
	}
	while( ++i < sizeof( int ) );

	push_bytearray( object, sizeof( int ) );
}

float pop_real()
{
	byte* object = pop_bytearray( sizeof( float ) );

	float value;

	memcpy( &value, object, sizeof(value) );

	return value;
}

void push_real( float value )
{
	byte object[ sizeof( float ) ];

	memcpy( object, &value, sizeof( value ) );

	push_bytearray( object, sizeof( float ) );
}

char pop_char()
{
	byte value = top_istack();

	pop_istack();

	return value;
}

void push_char( char value )
{
	push_istack( value );
}

char* pop_string( int size )
{
	return (char*) pop_bytearray( size );
}

void push_string( char* object )
{
	push_bytearray( object, strlen( object ) );
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

		int i;
		for( i = 0; i < osize; i++ )
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

		int i;
		for( i = 0; i < asize; i++ )
			astack[i] = old_astack[i];

		asize += ASTACK_UNIT;

		free( old_astack );
	}

	astack[ ap++ ] = value;
}
