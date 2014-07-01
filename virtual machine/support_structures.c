/*
 * File: support_structures.c
 * Description: VM stacks and methods to access them
 * Date: 140624
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "support_structures.h"

int pc;

int ap, op, ip;
int t_op, t_ip;
int asize, osize, isize;
int t_osize, t_isize; 

Adescr** astack;
Odescr** ostack;
byte* istack;
map_t string_map;

Odescr** t_ostack;
byte* t_istack;

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
	t_istack = malloc( ISTACK_UNIT );
	
	asize = 1;
	osize = 1;
	isize = 1;

	t_osize = 1;
	t_isize = 1;

	string_map = hashmap_new();

	return MEM_OK;
}

// Interaction with the persistent stacks 
byte top_istack()
{
#ifdef DEBUG
	fprintf( stderr, "Top istack, position %d\n", ip - 1 );
#endif

	return istack[ ip - 1 ]; 
}

void deallocate_istack( int size )
{
	ip -= size;
#ifdef DEBUG
	fprintf( stderr, "Deallocated istack, new ip %d\n", ip );
#endif
}

void allocate_istack( int size )
{
	if( ip + size - 1 >= isize )
    {
        isize += size;
		istack = realloc( istack, ISTACK_UNIT * isize );
    }

    while( size-- > 0 )
      istack[ ip++ ] = 0;

#ifdef DEBUG
	fprintf( stderr, "Allocated istack (ip %d isize %d)\n", ip, isize );
#endif
}

Odescr* top_ostack()
{
	Odescr* value = ostack[ op - 1 ];

	return value;
}

void pop_ostack()
{
#ifdef DEBUG
	fprintf( stderr, "Pop ostack: %d\n", op );
#endif
	free( ostack[ --op ] );
}

void push_ostack( Odescr* value )
{
	if( op == osize )
	{
#ifdef DEBUG
		fprintf( stderr, "Have to reallocate ostack.\n" );
#endif
		ostack = realloc( ostack, OSTACK_UNIT * ++osize );
	}

#ifdef DEBUG
	fprintf( stderr, "Push ostack @ %d: %d %d ", op, value->mode, value->size );
	if( value->mode == EMB )
		fprintf( stderr, "%p", value->inst.emb_val );
	else
		fprintf( stderr, "%d", value->inst.sta_val );
	fprintf( stderr, " in %p.\n", value );
#endif

	ostack[ op++ ] = value;
}

void enlarge_ostack( int size )
{
	if( op + size <= osize )
	{
#ifdef DEBUG
		fprintf( stderr, "No need to realloc ostack to host new function objects\n" );
#endif
		return;
	}

#ifdef DEBUG
	fprintf( stderr, "Have to reallocate ostack to host new function objects\n");
#endif
	ostack = realloc( ostack, OSTACK_UNIT * ( op + size ) );
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

// Interactions with the temporary stacks
Odescr* top_t_ostack()
{
	Odescr* value = t_ostack[ t_op - 1 ];

#ifdef DEBUG
	fprintf( stderr, "Top t_ostack position %d: %d %d %d\n", t_op - 1, value->mode, value->size, value->inst.sta_val );
#endif

	return value;
}

void pop_t_ostack()
{
#ifdef DEBUG
	fprintf( stderr, "Pop t_ostack: %d\n", t_op );
#endif
	free( t_ostack[ --t_op ] );
}

void push_t_ostack( Odescr* value )
{
	if( t_op == t_osize )
		t_ostack = realloc( t_ostack, OSTACK_UNIT * ++t_osize );

#ifdef DEBUG
	fprintf( stderr, "Push t_ostack @ %d: %d %d ", t_op, value->mode, value->size );
	if( value->mode == EMB )
		fprintf( stderr, "%p", value->inst.emb_val );
	else
		fprintf( stderr, "%d", value->inst.sta_val );
	fprintf( stderr, " in %p.\n", value );
#endif

	t_ostack[ t_op++ ] = value;
}

byte top_t_istack()
{
#ifdef DEBUG
	fprintf( stderr, "Top t_istack, position %d\n", t_ip - 1 );
#endif

	return t_istack[ t_ip - 1 ]; 
}

void pop_t_istack()
{
	--t_ip;
#ifdef DEBUG
	fprintf( stderr, "Popped t_istack, new t_ip %d\n", t_ip );
#endif
}

void push_t_istack( byte value )
{
	if( t_ip == t_isize )
		t_istack = realloc( t_istack, ISTACK_UNIT * ++t_isize );

	t_istack[ t_ip++ ] = value;

#ifdef DEBUG
	fprintf( stderr, "Pushed t_istack: %d (t_ip %d t_isize %d)\n", value, t_ip, t_isize );
#endif
}

// Write and read temporary stuff on the t_istack and the t_ostack;
// All methods pass from the bytearray ones
ByteArray pop_bytearray()
{
	Odescr* object = top_t_ostack();

	ByteArray result;
    result.value = malloc( sizeof( byte ) * object->size );
	result.size = object->size;

#ifdef DEBUG
	fprintf( stderr, "******************%d\n", object->size );
#endif

	int i = object->size - 1;
	do
	{
		result.value[ i ] = top_t_istack();

		pop_t_istack();
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
		push_t_istack( value[ i ] );

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = size;
	object->inst.sta_val = t_ip - size;

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
#ifdef DEBUG
	fprintf( stderr, "Push int: %d (%lu)\n", value, sizeof( value ) );
#endif
	
	byte object[ sizeof( value ) ];
	
	int i = 0;

	memcpy( object, &value, sizeof( value ) );
	
#ifdef DEBUG
	for( i = 0; i < sizeof( value ); i++ )
		fprintf( stderr, "Byte %d: %X\n", i, object[i] );
#endif

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

// Conversions
ByteArray encrypt_bytearray( ByteArray* array, char* format )
{
	ByteArray result;
	result.value = malloc( sizeof( byte ) );
	result.size = 0;

	adjust_bytearray( array, &result, format, 'e' );

	return result;
}

ByteArray decrypt_bytearray( ByteArray* array, char* format )
{
	ByteArray result;
	result.value = malloc( sizeof( byte ) );
	result.size = 0;

	adjust_bytearray( array, &result, format, 'd' );

	return result;
}

char* adjust_bytearray( ByteArray* array, ByteArray* result, char* format, char type )
{
#ifdef DEBUG
	fprintf( stderr, "Starting '%c' crypting '%s'  %lu long with '%s' format\n", type, array->value, array->size, format );
	fprintf( stderr, "Processing '%c'\n", *format );
#endif
	switch( *(format) )
	{
		case '(':
			while( *(format) != ')' )
			{
#ifdef DEBUG
                fprintf( stderr, "starting from %c\n", *format );
#endif
				while( *(format) != ':' )
					format++;
                format++;

				format = adjust_bytearray( array, result, format, type );
			}
			break;

		case '[':
		{
#ifdef DEBUG
			fprintf( stderr, "+++ Array start +++\n" );
#endif
			format++;
			int array_times, i;
			array_times = 0;
			while( *(format) != ',' )
			{
				array_times = ( array_times * 10 ) + ( *(format) - '0' );
				format++;
			}
			format++;

#ifdef DEBUG
			fprintf( stderr, "Vectoring %d times \n", array_times );
#endif
			char* array_format = malloc( sizeof( char ) * ( strlen( format ) + 1 ) );
			memcpy( array_format, format, sizeof( char ) * ( strlen( format ) + 1 ) );
			char* iterator = array_format;
			int sub_arrays = 0;
			while( sub_arrays >= 0 )
			{
				if( *iterator == '[' )
					sub_arrays++;
				
				if( *iterator == ']' )
					sub_arrays--;
				
				iterator++;
			} 
			*(--iterator) = '\0';

			for( i = 0; i < array_times; i++ )
				adjust_bytearray( array, result, array_format, type );

			// Skipping the size of the format read and the closed ']'
			format += ( sizeof( char ) * strlen( array_format ) ) + 1;
			break;
		}

		case 'i':
		{
			int i;
			result->value = realloc( result->value, result->size + sizeof( int ) );
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
			result->value = realloc( result->value, result->size + sizeof( float ) );
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
			result->value = realloc( result->value, result->size + sizeof( char ) );
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
#ifdef DEBUG
			fprintf( stderr, "Nothing to do with '%c'\n", *(format) );
#endif
			format++;
			break;
	}
	

#ifdef DEBUG
	fprintf( stderr, "Finished '%c' crypting '%s' %lu long\n", type, result->value, result->size );
#endif
	return format;
}

void encrypt_string( ByteArray* array, ByteArray* result )
{
	// I have the string, I'll insert it in the hashmap and then substituting
	// it in the result with its pointer
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

	// Saving the size of the string that has to be added, reused in realloc and in result->size
	unsigned long str_size = sizeof( char ) * ( strlen( str_pointer ) + 1 );
	result->value = realloc( result->value, result->size + str_size );
	memcpy( 
		&( result->value[ result->size ] ),
		str_pointer,
		str_size );
	result->size += str_size;
}
