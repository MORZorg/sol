/*
 * File: executor.c
 * Description: Most methods of the virtual machine
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "executor.h"

extern Stat* program;

int yyvm( void )
{
	int result;
	initialize_gui();
	initialize_stacks();

	while( program[pc].op != SOL_HALT )
	{
		printf( "Line %d, command %s (%d).\n", pc, get_operator_name( program[pc].op ), program[pc].op );

		if( ( result = execute( program[pc] ) != 0 ) )
		{
			printf( "Error number: %d\n", result );
			return result;
		}

		pc++;
	}

	finalize_gui();

	return MEM_OK;
}

int execute( Stat current_statement )
{
	switch( current_statement.op )
	{
		case SOL_NEW:
			return sol_new( current_statement.args );

		case SOL_NEWS:
			return sol_news( current_statement.args );

		case SOL_LDC:
			return sol_ldc( current_statement.args );

		case SOL_LDI:
			return sol_ldi( current_statement.args );

		case SOL_LDR:
			return sol_ldr( current_statement.args );

		case SOL_LDS:
			return sol_lds( current_statement.args );

		case SOL_LOD:
			return sol_lod( current_statement.args );

		case SOL_CAT:
			return sol_cat( current_statement.args );

		case SOL_LDA:
			return sol_lda( current_statement.args );

		case SOL_FDA:
			return sol_fda( current_statement.args );

		case SOL_EIL:
			return sol_eil( current_statement.args );

		case SOL_SIL:
			return sol_sil( current_statement.args );

		case SOL_IXA:
			return sol_ixa( current_statement.args );

		case SOL_STO:
			return sol_sto( current_statement.args );

		case SOL_IST:
			return sol_ist();

		case SOL_JMF:
			return sol_jmf( current_statement.args );

		case SOL_JMP:
			return sol_jmp( current_statement.args );

		case SOL_EQU:
			return sol_equ();

		case SOL_NEQ:
			return sol_neq();

		case SOL_CGT:
		case SOL_CGE:
		case SOL_CLT:
		case SOL_CLE:
			return sol_char_compare( current_statement.op );

		case SOL_IGT:
		case SOL_IGE:
		case SOL_ILT:
		case SOL_ILE:
			return sol_int_compare( current_statement.op );

		case SOL_RGT:
		case SOL_RGE:
		case SOL_RLT:
		case SOL_RLE:
			return sol_real_compare( current_statement.op );

		case SOL_SGT:
		case SOL_SGE:
		case SOL_SLT:
		case SOL_SLE:
			return sol_string_compare( current_statement.op );

		case SOL_IN:
			return sol_in();

		case SOL_IPLUS:
		case SOL_IMINUS:
		case SOL_ITIMES:
		case SOL_IDIV:
			return sol_int_math( current_statement.op );

		case SOL_RPLUS:
		case SOL_RMINUS:
		case SOL_RTIMES:
		case SOL_RDIV:
			return sol_real_math( current_statement.op );

		case SOL_IUMI:
			return sol_iumi();

		case SOL_RUMI:
			return sol_rumi();

		case SOL_NEG:
			return sol_neg();

		case SOL_WR:
			return sol_wr( current_statement.args );

		case SOL_FWR:
			return sol_fwr( current_statement.args );

		case SOL_PUSH:
			return sol_push( current_statement.args );

		case SOL_GOTO:
			return sol_goto( current_statement.args );

		case SOL_POP:
			return sol_pop();

		case SOL_RD:
			return sol_rd( current_statement.args );

		case SOL_FRD:
			return sol_frd( current_statement.args );

		case SOL_TOINT:
			return sol_toint();

		case SOL_TOREAL:
			return sol_toreal();

		case SOL_READ:
			return sol_read( current_statement.args );

		case SOL_FREAD:
			return sol_fread( current_statement.args );

		case SOL_WRITE:
			return sol_write( current_statement.args );

		case SOL_FWRITE:
			return sol_fwrite( current_statement.args );

		case SOL_FUNC:
			return sol_func( current_statement.args );

		case SOL_RETURN:
			return sol_return();

		default:
			return sol_noop();
	}

	return MEM_OK;
}

// Execution of S-code instructions
// Creates a new empty embedded object and puts it on ostack
int sol_new( Value* args )
{
	int size = args[ 0 ].i_val;

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = EMB;
	object->size = size;
	object->inst.emb_val = malloc( sizeof( byte ) * size );

	push_ostack( object );

	return MEM_OK;
}

// Creates a new empty stack object and puts it on ostack
// A size number of cells is reserved on the istack
int sol_news( Value* args )
{
	int size = args[ 0 ].i_val;

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = size;
	object->inst.sta_val = ip;
    allocate_istack( size );

	push_ostack( object );

	return MEM_OK;
}

// Loads the given char on t_istack
int sol_ldc( Value* args )
{
	push_char( args[ 0 ].c_val );

	return MEM_OK;
}

// Loads the given int on t_istack
int sol_ldi( Value* args )
{
	push_int( args[ 0 ].i_val );

	return MEM_OK;
}

// Loads the given real on t_istack
int sol_ldr( Value* args )
{
	push_real( args[ 0 ].r_val );

	return MEM_OK;
}

// Loads the given string on t_istack
int sol_lds( Value* args )
{
	push_string( args[ 0 ].s_val );

	return MEM_OK;
}

// Retrieves the value of the given object (referred with its oid) and loads its value on the temporary stack (copies it from the istack if the object if STA)
int sol_lod( Value* args )
{
	int env_offset = args[ 0 ].i_val;
	int oid = args[ 1 ].i_val - 1;
	int env = ap - 1;
	int i;
	Odescr* object;
	ByteArray instance;

	while( env_offset-- > 0 )
		env = astack[ env ]->alink;

	if( env < 0 )
		return ERROR_ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ env ]->obj_number )
		return ERROR_OSTACK_OUT_OF_BOUND;

	fprintf( stderr, "I'm gonna load the value at env %d with oid %d!\n", env, oid );
	object = ostack[ astack[ env ]->first_object + oid ];
	fprintf( stderr, "I took %p.\n", object);

	if( object->mode == EMB )
		push_bytearray( object->inst.emb_val, object->size );
	else
	{
		instance.value = malloc( sizeof( byte ) * object->size );

		for( i = 0; i < object->size; i++ )
			instance.value[ i ] = istack[ object->inst.sta_val + i ];

		push_bytearray( instance.value, object->size );
	}

	return MEM_OK;
}

// Creates a temporary object which refers to a structure or vector composed by the last element_number elements on the stack. Total size is specified as parameter too
// It's different from NEWS because the value is instantiated immediately
int sol_cat( Value* args )
{
	int element_number = args[ 0 ].i_val;
	int total_size = args[ 1 ].i_val;
	Odescr* object;

	// Remove the descriptor of the single elements
	// A descriptor of the whole structure is created as purpose of this instruction 
	while( element_number-- > 0 )
		pop_t_ostack();

	object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = total_size;
	object->inst.sta_val = ip - total_size;

	push_t_ostack( object );

	return MEM_OK;
}

// Retrieves the starting address of the value of the given stack object and loads it on the stack as an integer
int sol_lda( Value* args )
{
	int env_offset = args[ 0 ].i_val;
	int oid = args[ 1 ].i_val - 1;
	int env = ap - 1;
	Odescr* object;

	while( env_offset-- > 0 )
		env = astack[ env ]->alink;

	if( env < 0 )
		return ERROR_ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ env ]->obj_number )
		return ERROR_OSTACK_OUT_OF_BOUND;

	object = ostack[ astack[ env ]->first_object + oid ];

	push_int( object->inst.sta_val );

	return MEM_OK;
}

// Updates the address loaded by LDA with an offset of given size
int sol_fda( Value* args )
{
	int field_offset = args[ 0 ].i_val;

	int ref_offset_on_stack = pop_int(); // Field offset previously calculated (either LDA or other FDAs)

	push_int( field_offset + ref_offset_on_stack );

	return MEM_OK;
}

// Identical to FDA but specific for vectors
// The only given parameter is the size of the vector's elements dimension (eg vector [ 10 ] of int has dimension |int|, while vector [ 10 ] of vector [ 20 ] of int has 2 dimensions which elements have size 20*|int| and |int|)
// The index value must have been previously calculated (and therefore be present as last value on the t_istack), the same applies for the base address of the vector, loaded with a LDA
int sol_ixa( Value* args )
{
	int vector_dimension = args[ 0 ].i_val;

	int index_value = pop_int(); // Index value calculated via expression
	int start_offset = pop_int(); // Previously calculated offset (either LDA or other IXAs)

	push_int( start_offset + vector_dimension * index_value );

	return MEM_OK;
}

// Gets the start_offset from the t_istack, previously calculated with LDA and various FDA (or IXA), goes on the istack to the field, copies it in its entirety and puts it on the stack
int sol_eil( Value* args )
{
	int field_size = args[ 0 ].i_val;
	int start_offset = pop_int(); // Offset calculated via multiple IXA/FDA + LDA
	byte* value;
	int i = 0;

	if( start_offset + field_size > ip - 1 )
		return ERROR_ISTACK_OUT_OF_BOUND;

	value = malloc( sizeof( byte ) * field_size );

	do
	{
		value[ i ] = istack[ start_offset + i ];
	}
	while( ++i < field_size );

	push_bytearray( value, field_size );

	return MEM_OK;
}

// TODO not sure - for now it's the same as EIL
// Not clear: what should be put on the istack? the address of the field (start_offset, already present)?
int sol_sil( Value* args )
{
	int field_size = args[ 0 ].i_val;
	int start_offset = pop_int(); // Offset calculated via multiple IXA/FDA + LDA
	byte* value;
	int i;

	if( start_offset + field_size > ip - 1 )
		return ERROR_ISTACK_OUT_OF_BOUND;

	value = malloc( sizeof( byte ) * field_size );
	i = 0;

	// Mh wat?
	do
	{
		value[ i ] = istack[ start_offset + i ];
	}
	while( ++i < field_size );

	push_bytearray( value, field_size );


	return MEM_OK;
}

// Pops the last value from the t_istack and puts it as instance of the referred object, whose size is known as part of Odescr
int sol_sto( Value* args )
{
	int env_offset = args[ 0 ].i_val;
	int oid = args[ 1 ].i_val - 1;
	int env = ap - 1;
	int i;
	Odescr* object;
	ByteArray instance;

	while( env_offset-- > 0 )
		env = astack[ env ]->alink;

	fprintf( stderr, "I'm gonna store the value at env %d with oid %d!\n", env, oid );

	if( env < 0 )
		return ERROR_ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ env ]->obj_number )
		return ERROR_OSTACK_OUT_OF_BOUND;

	object = ostack[ astack[ env ]->first_object + oid ];
	fprintf( stderr, "I took %p.\n", object);

	if( object->mode == EMB )	
		object->inst.emb_val = pop_bytearray().value;
	else
	{
		// Copy the last value on the t_istack at the position of the sta_val of
		// the given array or structure, w/e
		instance = pop_bytearray();

		for( i = 0; i < instance.size; i++ )
			istack[ object->inst.sta_val + i ] = instance.value[ i ];
	}

	return MEM_OK;
}

// Reads the value to assign, the field/index address and instantiates the field/index
int sol_ist()
{
	ByteArray value_descriptor = pop_bytearray();
	int start_address = pop_int();
	int i;

	fprintf( stderr, "Imma write from %d for %ld bytes, the istack is this big: %d!\n", start_address, value_descriptor.size, isize );

	for( i = 0; i < value_descriptor.size; i++ )
		istack[ start_address + i ] = value_descriptor.value[ i ];

	return MEM_OK;
}

// If the last value on the stack is false, jump
// NOTE booleans are implemented as integers -> 0 = false, 1 = true
// NOTE The jump is pc += offset - 1 because pc++ is always executed after every call of execute( Stat* )
int sol_jmf( Value* args )
{
	int jump_offset = args[ 0 ].i_val;

	if( pop_char() == FALSE )
		pc += jump_offset - 1;

	return MEM_OK;
}

// Unconditioned jump
int sol_jmp( Value* args )
{
	int jump_offset = args[ 0 ].i_val;

	pc += jump_offset - 1;

	return MEM_OK;
}

// == operator
int sol_equ()
{
	ByteArray left_object = pop_bytearray();
	ByteArray right_object = pop_bytearray();
	int i;

	if( left_object.size != right_object.size )
	{
		push_char( FALSE );
		return MEM_OK;
	}

	for( i = 0; i < left_object.size; i++ )
		if( left_object.value[ i ] != right_object.value[ i ] )
		{
			push_char( FALSE );
			return MEM_OK;
		}

	push_char( TRUE );

	return MEM_OK;
}

// != operator
int sol_neq()
{
	ByteArray left_object = pop_bytearray();
	ByteArray right_object = pop_bytearray();

	int i;
	for( i = 0; i < left_object.size; i++ )
		if( left_object.value[ i ] != right_object.value[ i ] )
		{
			push_char( TRUE );
			return MEM_OK;
		}

	push_char( FALSE );

	return MEM_OK;
}

// BORING
int sol_char_compare( Operator op )
{
	char right_value = pop_char();
	char left_value = pop_char();

	if( op == SOL_CGT && left_value > right_value )
		push_char( TRUE );
	else if( op == SOL_CGE && left_value >= right_value )
		push_char( TRUE );
	else if( op == SOL_CLT && left_value < right_value )
		push_char( TRUE );
	else if( op == SOL_CLE && left_value <= right_value )
		push_char( TRUE );

	push_char( FALSE );

	return MEM_OK;
}

int sol_int_compare( Operator op )
{
	int right_value = pop_int();
	int left_value = pop_int();

	if( op == SOL_IGT && left_value > right_value )
		push_char( TRUE );
	else if( op == SOL_IGE && left_value >= right_value )
		push_char( TRUE );
	else if( op == SOL_ILT && left_value < right_value )
		push_char( TRUE );
	else if( op == SOL_ILE && left_value <= right_value )
		push_char( TRUE );
	else
		push_char( FALSE );

	return MEM_OK;
}

int sol_real_compare( Operator op )
{
	float right_value = pop_real();
	float left_value = pop_real();

	if( op == SOL_RGT && left_value > right_value )
		push_char( TRUE );
	else if( op == SOL_RGE && left_value >= right_value )
		push_char( TRUE );
	else if( op == SOL_RLT && left_value < right_value )
		push_char( TRUE );
	else if( op == SOL_RLE && left_value <= right_value )
		push_char( TRUE );
	else
		push_char( FALSE );

	return MEM_OK;
}

int sol_string_compare( Operator op )
{
	char* right_value = pop_string();
	char* left_value = pop_string();

	if( op == SOL_SGT && strcmp( left_value, right_value ) > 0 )
		push_char( TRUE );
	else if( op == SOL_SGE && strcmp( left_value, right_value ) >= 0 )
		push_char( TRUE );
	else if( op == SOL_SLT && strcmp( left_value, right_value ) < 0 )
		push_char( TRUE );
	else if( op == SOL_SLE && strcmp( left_value, right_value ) <= 0 )
		push_char( TRUE );
	else
		push_char( FALSE );

	return MEM_OK;
}

// The check is done byte-by-byte
// FIXME WRONG.   "pino" in vector( "campi", "nonno argald" ) == true
int sol_in()
{
	ByteArray set = pop_bytearray();
	ByteArray value = pop_bytearray();

	int i, j;
	for( i = 0; i < set.size - value.size; i++ )
	{
		if( set.value[ i ] == value.value[ 0 ] )
		{
			for( j = 1; j < value.size; j++ )
			{
				if( set.value[ i + j ] != value.value[ j ] )
				{
					push_char( FALSE );
					break;
				}
			}

			if( j == value.size )
			{
				push_char( TRUE );
				break;
			}
		}
	}

	if( i == set.size - value.size )
		push_char( FALSE );

	return MEM_OK;
}

// BORING
int sol_int_math( Operator op )
{
	int right_value = pop_int();
	int left_value = pop_int();

	int result = 0;

	if( op == SOL_IPLUS )
		result = left_value + right_value;
	else if( op == SOL_IMINUS )
		result = left_value - right_value;
	else if( op == SOL_ITIMES )
		result = left_value * right_value;
	else if( op == SOL_IDIV )
		result = left_value / right_value;

	push_int( result );

	return MEM_OK;
}

int sol_real_math( Operator op )
{
	float right_value = pop_real();
	float left_value = pop_real();

	float result = 0;

	if( op == SOL_RPLUS )
		result = left_value + right_value;
	else if( op == SOL_RMINUS )
		result = left_value - right_value;
	else if( op == SOL_RTIMES )
		result = left_value * right_value;
	else if( op == SOL_RDIV )
		result = left_value / right_value;

	push_real( result );

	return MEM_OK;
}

// Negations
int sol_iumi()
{
	push_int( -1 * pop_int() );

	return MEM_OK;
}

int sol_rumi()
{
	push_real( -1 * pop_real() );

	return MEM_OK;
}

int sol_neg()
{
	if( pop_char() == FALSE )
		push_char( TRUE );
	else
		push_char( FALSE );

	return MEM_OK;
}

// Push the chain and element_number on the t_istack, in preaparation of the call to GOTO, and instantiate a new activation record
int sol_push( Value* args )
{
	int element_number = args[ 0 ].i_val;
	int chain = args[ 1 ].i_val;

	enlarge_ostack( element_number );

	push_int( element_number );
	fprintf( stderr, "SOL pushed el#: %d\n", element_number );
	push_int( chain );
	fprintf( stderr, "SOL pushed chain: %d\n", chain );

	return MEM_OK;
}

// GOTO is used ONLY after a push, to perform a function call
int sol_goto( Value* args )
{
	int entry_point = args[ 0 ].i_val;
	int chain = pop_int();
	int element_number = pop_int();
	Adescr* function_ar;

	fprintf( stderr, "SOL goto chain: %d\n", chain );
	fprintf( stderr, "SOL goto el#: %d\n", element_number );

	// The number of elements is given, the start point for its objects is the
	// top of the stack (the objects will be instantiated as part of the
	// function call, not before)
	function_ar = malloc( sizeof( Adescr ) );
	function_ar->obj_number = element_number;
	function_ar->first_object = op;
	function_ar->raddr = pc + 1;
	function_ar->alink = ap - 1;
	while( chain-- > 0 )
		function_ar->alink = astack[ function_ar->alink ]->alink;
	
	push_astack( function_ar );

	// Jump to the entry point (first instruction will be the definition of the formals)
	pc = entry_point - 1;

	return MEM_OK;
}

// Clean the stacks after the last function call
// FIXME now the eventually temporary stuff present is left on the t_istack. Does this create some problem?
// Is there anything at all?
int sol_pop()
{
	int i;

	for( i = 0; i < top_astack()->obj_number; i++ )
	{
		// All the instances of the current environment are on top of the
		// istack, all I care about is to pop the correct total number of
		// cells, not the exact cells for every object
		if( top_ostack()->mode == STA )
          deallocate_istack( top_ostack()->size );

		pop_ostack();
	}

	pop_astack();

	return MEM_OK;
}

// Write on the std output in the given format
// Leaves the expr result available on the t_istack
int sol_wr( Value* args )
{
	char* format = args[ 0 ].s_val;
	ByteArray popped = pop_bytearray();
	ByteArray expr;

	decrypt_bytearray( &popped, &expr, format );

	userOutput( format, expr );

	push_bytearray( expr.value, expr.size );

	return MEM_OK;
}

// Leaves the expr result available on the t_istack
int sol_fwr( Value* args )
{
	char* format = args[ 0 ].s_val;
	char* filename = pop_string();
	ByteArray popped = pop_bytearray();
	ByteArray expr;

	decrypt_bytearray( &popped, &expr, format );

	fileOutput( filename, expr );

	push_bytearray( expr.value, expr.size );

	return MEM_OK;
}

// Read input from user and save it in the lhs object
// Leaves the input result available on the t_istack
int sol_rd( Value* args )
{
	char* format = args[ 0 ].s_val;
	ByteArray input = userInput( format );
	ByteArray result;
	
	result.value = malloc( sizeof( byte ) );
	result.size = 0;
	encrypt_bytearray( &input, &result, format );

	push_bytearray( result.value, result.size );

	return MEM_OK;
}

// Same as rd but reads from file
// Leaves the input result available on the t_istack
int sol_frd( Value* args )
{
	char* format = args[ 0 ].s_val;
	char* filename = pop_string();
	ByteArray input = fileInput( filename );
	ByteArray result;

	result.value = malloc( sizeof( byte ) );
	result.size = 0;
	encrypt_bytearray( &input, &result, format );

	push_bytearray( result.value, result.size );

	return MEM_OK;
}

// From real to int
int sol_toint()
{
	float value = pop_real();

	push_int( (int) value );

	return MEM_OK;
}

// From int to real
int sol_toreal()
{
	int value = pop_int();

	push_real( (float) value );

	return MEM_OK;
}

// RD, FRD, WR, FWR but without leaving the value on the stack
// and with assignment to a variable in the case of f/read
int sol_read( Value* args )
{
	int env_offset = args[ 0 ].i_val;
	int oid = args[ 1 ].i_val - 1;
	char* format = args[ 2 ].s_val;
	int env = ap - 1;
	Odescr* lhs;
	ByteArray input = userInput( format );
	ByteArray result;

	result.value = malloc( sizeof( byte ) );
	result.size = 0;

	while( env_offset-- > 0 )
		env = astack[ env ]->alink;

	encrypt_bytearray( &input, &result, format );

	lhs = ostack[ astack[ env ]->first_object + oid ];

	if( lhs->mode == EMB )
		lhs->inst.emb_val = result.value;
	else
	{
		lhs->inst.sta_val = ip;
		push_bytearray( result.value, result.size );
		pop_ostack();
	}

	return MEM_OK;
}

int sol_fread( Value* args )
{
	int env_offset = args[ 0 ].i_val;
	int oid = args[ 1 ].i_val - 1;
	char* format = args[ 2 ].s_val;
	int env = ap - 1;
	char* filename = pop_string();
	Odescr* lhs;
	ByteArray input = fileInput( filename );
	ByteArray result;

	result.value = malloc( sizeof( byte ) );
	result.size = 0;

	while( env_offset-- > 0 )
		env = astack[ env ]->alink;
	
	encrypt_bytearray( &input, &result, format );

	lhs = ostack[ astack[ env ]->first_object + oid ];

	if( lhs->mode == EMB )
		lhs->inst.emb_val = result.value;
	else
	{
		lhs->inst.sta_val = ip;
		push_bytearray( result.value, result.size );
		pop_ostack();
	}

	return MEM_OK;
}

int sol_write( Value* args )
{
	char* format = args[ 0 ].s_val;
	ByteArray popped = pop_bytearray();
	ByteArray expr;
	expr.value = malloc( sizeof( byte ) );
	expr.size = 0;

	decrypt_bytearray( &popped, &expr, format );
	userOutput( format, expr );

	return MEM_OK;
}

int sol_fwrite( Value* args )
{
	char* format = args[ 0 ].s_val;
	char* filename = pop_string();
	ByteArray popped = pop_bytearray();
	ByteArray expr;
	expr.value = malloc( sizeof( byte ) );
	expr.size = 0;

	decrypt_bytearray( &popped, &expr, format );
	fileOutput( filename, expr );

	return MEM_OK;
}

// Nothing to do?
// Can't create an Odescr because, otherwise, the oid direct addressing would fail (functions are referred to with a different count)
int sol_func( Value* args )
{
	// int fid = args[ 0 ].i_val;

	return MEM_OK;
}

// Jump to the pop instruction
int sol_return()
{
	pc = astack[ ap - 1 ]->raddr - 1;

	return MEM_OK;
}

int sol_noop()
{
	return ERROR_UNRECOGNISED;
}
