/*
 * File: support_structures.c
 * Description: VM stacks and methods to access them
 * Date: 140624
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "support_structures.h"

int pc;

int ap, op, ip, t_op;
int asize, osize, isize, t_osize;

Adescr** astack;
Odescr** ostack;
byte* istack;
map_t string_map;

Odescr** t_ostack;

/*
 * PRIVATE FUNCTION
 */

/**
 * @brief Inserts an element checking name integrity in the rest of the scope.
 *
 * @param element
 *
 * @return 
 */
char* insert_unconflicted_element( char* string )
{
	any_t pointer;
	if( hashmap_get( string_map, string, &pointer ) == MAP_OK )
		return pointer;

	hashmap_put( string_map, string, string );
	return string;
}

/*
 * PUBLIC FUNCTIONS
 */

int initialize_stacks()
{
	pc = ap = op = ip = t_op = 0;

	astack = malloc( ASTACK_UNIT );
	ostack = malloc( OSTACK_UNIT );
	istack = malloc( ISTACK_UNIT );

	t_ostack = malloc( OSTACK_UNIT );
	
	asize = 1;
	osize = 1;
	isize = 1;

	t_osize = 1;

	string_map = hashmap_new();

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

// Write and read temporary stuff on the istack and the t_ostack;
// All methods pass from the bytearray ones
ByteArray pop_bytearray()
{
	Odescr* object = top_t_ostack();

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
	pop_t_ostack();

	return result;
}

void push_bytearray( byte* value, int size )
{
	int i;
	for( i = 0; i < size; i++ )
		push_istack( value[ i ] );

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = size;
	object->inst.sta_val = ip - size;

	push_t_ostack( object );
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

	push_bytearray( object, sizeof( value ) );
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
	byte* byte_pointer = pop_bytearray().value;
	char* pointer;
	memcpy( &pointer, byte_pointer, sizeof( pointer ) );
	return pointer;
}

void push_string( char* string )
{
	// Pushing the string in the hashmap and pushing its pointer to the ostack
	char* pointer = insert_unconflicted_element( string );
	byte object[ sizeof( pointer ) ];
	memcpy( object, &pointer, sizeof( pointer ) );
	push_bytearray( object, sizeof( pointer ) );
}

// Interactions with the other stacks
Odescr* top_ostack()
{
	Odescr* value = ostack[ op - 1 ];

	/* fprintf( stderr, "Top ostack position %d: %d %d %d\n", op - 1, value->mode, value->size, value->inst.sta_val ); */

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
	{
		printf("Have to reallocate ostack.\n");
		ostack = realloc( ostack, OSTACK_UNIT * ++osize );
	}

	fprintf( stderr, "Push ostack @ %d: %d %d ", op, value->mode, value->size );
	if( value->mode == EMB )
		fprintf( stderr, "%p", value->inst.emb_val );
	else
		fprintf( stderr, "%d", value->inst.sta_val );
	fprintf( stderr, " in %p.\n", value );

	ostack[ op++ ] = value;
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

Odescr* top_t_ostack()
{
	Odescr* value = t_ostack[ t_op - 1 ];

	fprintf( stderr, "Top t_ostack position %d: %d %d %d\n", t_op - 1, value->mode, value->size, value->inst.sta_val );

	return value;
}

void pop_t_ostack()
{
	fprintf( stderr, "Pop t_ostack: %d\n", t_op );
	free( t_ostack[ --t_op ] );
}

void push_t_ostack( Odescr* value )
{
	if( t_op == t_osize )
		t_ostack = realloc( t_ostack, OSTACK_UNIT * ++t_osize );

	fprintf( stderr, "Push t_ostack @ %d: %d %d ", t_op, value->mode, value->size );
	if( value->mode == EMB )
		fprintf( stderr, "%p", value->inst.emb_val );
	else
		fprintf( stderr, "%d", value->inst.sta_val );
	fprintf( stderr, " in %p.\n", value );

	t_ostack[ t_op++ ] = value;
}

void encrypt_bytearray( ByteArray* array, ByteArray* result, char* format )
{
	adjust_bytearray( array, result, format, 'e' );
}

void decrypt_bytearray( ByteArray* array, ByteArray* result, char* format )
{
	adjust_bytearray( array, result, format, 'd' );
}

void adjust_bytearray( ByteArray* array, ByteArray* result, char* format, char type )
{
	fprintf( stderr, "Starting '%c' crypting '%s'  %lu long with '%s' format\n", type, array->value, array->size, format );

	while( *(format) != '\0' )
	{
		fprintf( stderr, "Processing '%c'\n", *format );
		switch( *(format) )
		{
			case '"':
				format++;
				break;

			case '(':
				while( *(format) != ')' )
				{
					while( *(format) != ':' )
						format++;
					adjust_bytearray( array, result, format, type );
				}
				break;

			case '[':
			{
				format++;
				int array_times, i;
				array_times = 0;
				while( *(format) != ',' )
				{
					array_times = ( array_times * 10 ) + ( *(format) - '0' );
					format++;
				}
				format++;

				fprintf( stderr, "Vectoring %d times \n", array_times );
				// The loop is not done one time since the variable format is
				// not updated, so, after this block, there will be one more
				// execution.
				// Hopefully we'll never find 0-sized vectors.
				for( i = 1; i < array_times; i++ )
					adjust_bytearray( array, result, format, type );

				break;
			}

			case 'i':
			{
				int i;
				result->value = realloc( result->value, sizeof( int ) );
				for( i = 0; i < sizeof( int ); i++ )
					result->value[ result->size + i ] = array->value[ i ];
				result->size += sizeof( int );
				array->value = &( array->value[ sizeof( int ) ] );
				format++;
				break;
			}

			case 'r':
			{
				int i;
				result->value = realloc( result->value, sizeof( float ) );
				for( i = 0; i < sizeof( float ); i++ )
					result->value[ result->size + i ] = array->value[ i ];
				result->size += sizeof( float );
				array->value = &( array->value[ sizeof( float ) ] );
				format++;
				break;
			}

			case 'c':
			case 'b':
			{
				int i;
				result->value = realloc( result->value, sizeof( char ) );
				for( i = 0; i < sizeof( char ); i++ )
					result->value[ result->size + i ] = array->value[ i ];
				result->size += sizeof( char );
				array->value = &( array->value[ sizeof( char ) ] );
				format++;
				break;
			}

			case 's':
			{
				switch( type )
				{
					case 'e':
						encrypt_string( array, result );
						break;

					case 'd':
						decrypt_string( array, result );
						break;
				}

				format++;
				break;
			}

			default:
				fprintf( stderr, "Nothing to do with '%c'\n", *(format) );
				format++;
				break;
		}
	}

	fprintf( stderr, "Finished '%c' crypting '%s' %lu long\n", type, result->value, result->size );
}

void encrypt_string( ByteArray* array, ByteArray* result )
{
	// I have the string, I'll insert it in the hashmap and then substituting it in the result with its pointer
	char* str_copy = malloc( sizeof( char ) * ( strlen( array->value ) + 1 ) );
	memcpy(
		str_copy,
		array->value,
		sizeof( char ) * ( strlen( array->value ) + 1 ) );

	char* string = insert_unconflicted_element( str_copy );
	array->value += sizeof( char ) * ( strlen( string ) + 1 );

	result->value = realloc(
						result->value,
						sizeof( char* ) + result->size );

	memcpy(
		&( result->value[ result->size ] ),
		&string,
		sizeof( char* ) );

	result->size += sizeof( char* );
}

void decrypt_string( ByteArray* array, ByteArray* result )
{
	// Getting the pointer to the string from its bytes
	char* str_pointer;
	memcpy( &str_pointer, array->value, sizeof( char* ) );
	array->value += sizeof( char* );

	result->value = realloc( 
						result->value,
						sizeof( byte ) * ( strlen( str_pointer ) + 1 ) + result->size );
	memcpy( 
		&( result->value[ result->size ] ),
		str_pointer,
		sizeof( char ) * ( strlen( str_pointer ) + 1 ) );

	result->size += sizeof( char ) * ( strlen( str_pointer ) + 1 );
}
