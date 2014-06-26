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

	astack = malloc( ASTACK_UNIT );
	ostack = malloc( OSTACK_UNIT );
	istack = malloc( ISTACK_UNIT );
	
	asize = 1;
	osize = 1;
	isize = 1;

	return MEM_OK;
}

// Interaction with the istack
byte top_istack()
{
	fprintf( stderr, "Top istack, position %d\n", ip - 1 );

	return istack[ ip - 1 ]; 
}

void pop_istack()
{
	--ip;
	fprintf( stderr, "Popped istack, new ip %d\n", ip );
}

void push_istack( byte value )
{
	if( ip == isize )
		istack = realloc( istack, ISTACK_UNIT * ++isize );

	istack[ ip++ ] = value;

	fprintf( stderr, "Pushed istack: %d (ip %d isize %d)\n", value, ip, isize );
}

// Write and read stuff on the istack
// All methods pass from the bytearray ones
ByteArray pop_bytearray()
{
	Odescr* object = top_ostack();

	ByteArray result;
    result.value = malloc( sizeof( byte ) * object->size );
	result.size = object->size;

	fprintf( stderr, "******************%d\n", object->size );

	int i = object->size - 1;
	do
	{
		result.value[ i ] = top_istack();

		pop_istack();
	}
	while( --i >= 0 );
	
	// Must be last because it destroys the value referred by object
	pop_ostack();

	return result;
}

void push_bytearray( byte* value, int size )
{
	int i = 0;

	do
	{
		push_istack( value[ i ] );
	}
	while( ++i < size );

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = size;
	object->inst.sta_val = ip - size;

	push_ostack( object );
}

int pop_int()
{
	int value = 0;

	memcpy( &value, pop_bytearray().value, sizeof( value ) );

	return value;
}

void push_int( int value )
{
	fprintf( stderr, "Push int: %d (%lu)\n", value, sizeof( value ) );
	
	byte object[ sizeof( value ) ];
	
	int i = 0;

	memcpy( object, &value, sizeof( value ) );
	
	for( i = 0; i < sizeof( value ); i++ )
		fprintf( stderr, "Byte %d: %X\n", i, object[i] );

	push_bytearray( (byte*) &value, sizeof( value ) );
}

float pop_real()
{
	byte* object = pop_bytearray().value;

	float value;

	memcpy( &value, object, sizeof( value ) );

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
	return pop_bytearray().value[ 0 ];
}

void push_char( char value )
{
	push_bytearray( (byte*) &value, 1 );
}

char* pop_string()
{
	return pop_bytearray().value;
}

void push_string( char* object )
{
	push_bytearray( object, strlen( object ) + 1 );
}

// Interactions with the other stacks
Odescr* top_ostack()
{
	Odescr* value = ostack[ op - 1 ];

	fprintf( stderr, "Top ostack position %d: %d %d %d\n", op - 1, value->mode, value->size, value->inst.sta_val );

	return value;
}

void pop_ostack()
{
	fprintf( stderr, "Pop ostack: %d\n", op );
	free( ostack[ --op ] );
}

void push_ostack( Odescr* value )
{
	if( op == osize )
		ostack = realloc( ostack, OSTACK_UNIT * ++osize );

	ostack[ op++ ] = value;

	fprintf( stderr, "Push ostack: %d %d %d\n", value->mode, value->size, value->inst.sta_val );
}

Adescr* top_astack()
{
	return astack[ ap - 1 ];
}

void pop_astack()
{
	free( astack[ --ap ] );
}

void push_astack( Adescr* value )
{
	if( ap == asize )
		astack = realloc( astack, ASTACK_UNIT * ++asize );

	astack[ ap++ ] = value;
}
