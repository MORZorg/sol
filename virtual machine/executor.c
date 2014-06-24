/*
 * File: executor.c
 * Description: Most methods of the virtual machine
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "executor.h"

int yyvm( Stat** statements )
{
	initialize_stacks();

	while( statements[pc++]->op != SOL_HALT )
		execute( statements[pc] );

	return 0;
}

int execute( Stat* current_statement )
{
	switch( current_statement->op )
	{
		case SOL_NEW:
			sol_new( current_statement->args );
			break;

		case SOL_NEWS:
			sol_news( current_statement->args );
			break;

		case SOL_LDC:
			sol_ldc( current_statement->args );
			break;

		case SOL_LDI:
			sol_ldi( current_statement->args );
			break;

		case SOL_LDR:
			sol_ldr( current_statement->args );
			break;

		case SOL_LDS:
			sol_lds( current_statement->args );
			break;

		case SOL_LOD:
			sol_lod( current_statement->args );
			break;

		case SOL_CAT:
			sol_cat( current_statement->args );
			break;

		case SOL_LDA:
			sol_lda( current_statement->args );
			break;

		case SOL_FDA:
			sol_fda( current_statement->args );
			break;

		case SOL_EIL:
			sol_eil( current_statement->args );
			break;

		case SOL_SIL:
			sol_sil( current_statement->args );
			break;

		case SOL_IXA:
			sol_ixa( current_statement->args );
			break;

		case SOL_STO:
			sol_sto( current_statement->args );
			break;

		case SOL_IST:
			sol_ist( current_statement->args );
			break;

		case SOL_JMF:
			sol_jmf( current_statement->args );
			break;

		case SOL_JMP:
			sol_jmp( current_statement->args );
			break;

		case SOL_EQU:
			sol_equ( current_statement->args );
			break;

		case SOL_NEQ:
			sol_neq( current_statement->args );
			break;

		case SOL_CGT:
		case SOL_CGE:
		case SOL_CLT:
		case SOL_CLE:
			sol_char_compare( current_statement->op, current_statement->args );
			break;

		case SOL_IGT:
		case SOL_IGE:
		case SOL_ILT:
		case SOL_ILE:
			sol_int_compare( current_statement->op, current_statement->args );
			break;

		case SOL_RGT:
		case SOL_RGE:
		case SOL_RLT:
		case SOL_RLE:
			sol_real_compare( current_statement->op, current_statement->args );
			break;

		case SOL_SGT:
		case SOL_SGE:
		case SOL_SLT:
		case SOL_SLE:
			sol_string_compare( current_statement->op, current_statement->args );
			break;

		case SOL_IN:
			sol_in( current_statement->args );
			break;

		case SOL_IPLUS:
		case SOL_IMINUS:
		case SOL_ITIMES:
		case SOL_IDIV:
			sol_int_math( current_statement->op, current_statement->args );
			break;

		case SOL_RPLUS:
		case SOL_RMINUS:
		case SOL_RTIMES:
		case SOL_RDIV:
			sol_real_math( current_statement->op, current_statement->args );
			break;

		case SOL_IUMI:
			sol_iumi( current_statement->args );
			break;

		case SOL_RUMI:
			sol_rumi( current_statement->args );
			break;

		case SOL_NEG:
			sol_neg( current_statement->args );
			break;

		case SOL_WR:
			sol_wr( current_statement->args );
			break;

		case SOL_FWR:
			sol_fwr( current_statement->args );
			break;

		case SOL_PUSH:
			sol_push( current_statement->args );
			break;

		case SOL_GOTO:
			sol_goto( current_statement->args );
			break;

		case SOL_POP:
			sol_pop( current_statement->args );
			break;

		case SOL_RD:
			sol_rd( current_statement->args );
			break;

		case SOL_FRD:
			sol_frd( current_statement->args );
			break;

		case SOL_TOINT:
			sol_toint( current_statement->args );
			break;

		case SOL_TOREAL:
			sol_toreal( current_statement->args );
			break;

		case SOL_READ:
			sol_read( current_statement->args );
			break;

		case SOL_FREAD:
			sol_fread( current_statement->args );
			break;

		case SOL_WRITE:
			sol_write( current_statement->args );
			break;

		case SOL_FWRITE:
			sol_fwrite( current_statement->args );
			break;

		case SOL_FUNC:
			sol_func( current_statement->args );
			break;

		case SOL_RETURN:
			sol_return( current_statement->args );
			break;

		case SOL_SCODE:
			sol_scode( current_statement->args );
			break;

		case SOL_HALT:
			sol_halt( current_statement->args );
			break;

		default:
			sol_noop();
			break;
	}

	return 0;
}

// Execution of S-code instructions
// Creates a new empty embedded object and puts it on ostack
int sol_new( Value* args )
{
	int size = args[0].i_val;

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = EMB;
	object->size = size;
	object->inst.emb_val = malloc( sizeof( byte ) * size );

	push_ostack( object );

	return 0;
}

// Creates a new empty stack object and puts it on ostack
int sol_news( Value* args )
{
	int size = args[0].i_val;

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = size;

	push_ostack( object );

	return 0;
}

// Loads the given char on istack
int sol_ldc( Value* args )
{
	push_char( args[0].c_val );

	return 0;
}

// Loads the given int on istack
int sol_ldi( Value* args )
{
	push_int( args[0].i_val );

	return 0;
}

// Loads the given real on istack
int sol_ldr( Value* args )
{
	push_real( args[0].r_val );

	return 0;
}

// Loads the given string on istack
int sol_lds( Value* args )
{
	push_string( args[0].s_val );

	return 0;
}

// Retrieves the value of the given embedded object (referred with its oid) and loads its value on the stack
int sol_lod( Value* args )
{
	int env_offset = args[0].i_val;
	int oid = args[1].i_val;

	if( ap - 1 - env_offset < 0 )
		return ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ ap - 1 - env_offset ]->obj_number )
		return OSTACK_OUT_OF_BOUND;

	Odescr* object = &( astack[ ap - 1 - env_offset ]->objects[ oid ] );

	push_bytearray( object->inst.emb_val, object->size );

	return MEM_OK;
}

// Creates a temporary object which refers to a structure or vector composed by the last element_number elements on the stack. Total size is specified as parameter too
// It's different from NEWS because the value is instantiated immediately
int sol_cat( Value* args )
{
	int element_number = args[0].i_val;
	int total_size = args[1].i_val;

	// Remove the descriptor of the single elements
	// A descriptor of the whole structure is created as purpose of this instruction 
	while( element_number-- > 0 )
		pop_ostack();

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = STA;
	object->size = total_size;
	object->inst.sta_val = ip - total_size;

	push_ostack( object );

	return 0;
}

// Retrieves the starting address of the value of the given stack object and loads it on the stack as an integer
int sol_lda( Value* args )
{
	int env_offset = args[0].i_val;
	int oid = args[1].i_val;
	
	if( ap - 1 - env_offset < 0 )
		return ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ ap - 1 - env_offset ]->obj_number )
		return OSTACK_OUT_OF_BOUND;

	Odescr* source_object = &( astack[ ap - 1 - env_offset ]->objects[ oid ] );

	push_int( source_object->inst.sta_val );

	return MEM_OK;
}

// Updates the address loaded by LDA with an offset of given size
int sol_fda( Value* args )
{
	int field_offset = args[0].i_val;

	int ref_offset_on_stack = pop_int();

	push_int( field_offset + ref_offset_on_stack );

	return 0;
}

// Gets the start_offset from the istack, previously calculated with LDA and various FDA, goes on the istack to the field, copies it in its entirety and puts it on the stack
int sol_eil( Value* args )
{
	int field_size = args[0].i_val;

	int start_offset = pop_int();

	if( start_offset + field_size > ip - 1 )
		return ISTACK_OUT_OF_BOUND;

	byte* value;
	int i = 0;

	do
	{
		value[ i ] = istack[ start_offset + i ];
	}
	while( ++i < field_size );

	push_bytearray( value, field_size );

	return MEM_OK;
}

// TODO implement
// Not clear: what should be put on the istack? the address of the field (start_offset, already present)?
int sol_sil( Value* args )
{
	int field_size = args[0].i_val;

	int start_offset = pop_int();

	if( start_offset + field_size > ip - 1 )
		return ISTACK_OUT_OF_BOUND;

	byte* value;
	int i = 0;

	// Mh wat?

	return MEM_OK;
}

// Identical to FDA but specific for vectors
// The only given parameter is the size of the vector's elements dimension (eg vector [10] of int has dimension |int|, while vector [10] of vector [20] of int has 2 dimensions which elements have size 20*|int| and |int|)
// The index value must have been previously calculated (and therefore be present as last value on the istack), the same applies for the base address of the vector, loaded with a LDA
int sol_ixa( Value* args )
{
	int vector_dimension = args[0].i_val;

	int index_value = pop_int(); // index value
	int start_offset = pop_int(); // LDA

	push_int( vector_dimension * index_value + vector_dimension );

	return 0;
}

// Pops the last value from the istack and puts it as embedded instance of the referred object, whose size is known as part of Odescr
int sol_sto( Value* args )
{
	int env_offset = args[0].i_val;
	int oid = args[1].i_val;

	if( ap - 1 - env_offset < 0 )
		return ASTACK_OUT_OF_BOUND;

	if( oid >= astack[ ap - 1 - env_offset ]->obj_number )
		return OSTACK_OUT_OF_BOUND;

	Odescr* object = &( astack[ ap - 1 - env_offset ]->objects[ oid ] );

	object->inst.emb_val = pop_bytearray();

	return MEM_OK;
}

// Reads the value to assign, the field/index address and instantiates the field/index
int sol_ist()
{
	Odescr* object = top_ostack();
	int size = object->size;

	byte* value = pop_bytearray();
	
	int start_address = pop_int();
	int i;

	for( i = 0; i < size; i++ )
		istack[ start_address + i ] = value[ i ];

	return 0;
}

// If the last value on the stack is false, jump
// NOTE booleans are implemented as integers -> 0 = false, 1 = true
// NOTE The jump is pc += offset - 1 because pc++ is always executed after every call of execute( Stat* )
int sol_jmf( Value* args )
{
	int jump_offset = args[0].i_val;

	if( pop_char() == FALSE )
		pc += jump_offset - 1;

	return 0;
}

// Unconditioned jump
int sol_jmp( Value* args )
{
	int jump_offset = args[0].i_val;

	pc += jump_offset - 1;

	return 0;
}

// == operator
int sol_equ( Value* args )
{
	Odescr* left_object = top_ostack();
	int left_size = left_object->size;

	byte* left_value = pop_bytearray();

	Odescr* right_object = top_ostack();
	int right_size = right_object->size;

	byte* right_value = pop_bytearray();

	if( left_size != right_size )
	{
		push_char( FALSE );
		return MEM_OK;
	}

	int i;

	for( i = 0; i < left_size; i++ )
		if( left_value[ i ] != right_value[ i ] )
		{
			push_char( FALSE );
			return MEM_OK;
		}

	push_char( TRUE );

	return MEM_OK;
}

// != operator
int sol_neq( Value* args )
{
	Odescr* left_object = top_ostack();
	int left_size = left_object->size;

	byte* left_value = pop_bytearray();

	Odescr* right_object = top_ostack();
	int right_size = right_object->size;

	byte* right_value = pop_bytearray();

	int i;

	for( i = 0; i < left_size; i++ )
		if( left_value[ i ] != right_value[ i ] )
		{
			push_char( TRUE );
			return MEM_OK;
		}

	push_char( FALSE );

	return MEM_OK;
}

// BORING
int sol_char_compare( Operator op, Value* args )
{
	char left_value = pop_char();
	char right_value = pop_char();

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

int sol_int_compare( Operator op, Value* args )
{
	int left_value = pop_int();
	int right_value = pop_int();

	if( op == SOL_IGT && left_value > right_value )
		push_char( TRUE );
	else if( op == SOL_IGE && left_value >= right_value )
		push_char( TRUE );
	else if( op == SOL_ILT && left_value < right_value )
		push_char( TRUE );
	else if( op == SOL_ILE && left_value <= right_value )
		push_char( TRUE );

	push_char( FALSE );

	return MEM_OK;
}

int sol_real_compare( Operator op, Value* args )
{
	float left_value = pop_real();
	float right_value = pop_real();

	if( op == SOL_RGT && left_value > right_value )
		push_char( TRUE );
	else if( op == SOL_RGE && left_value >= right_value )
		push_char( TRUE );
	else if( op == SOL_RLT && left_value < right_value )
		push_char( TRUE );
	else if( op == SOL_RLE && left_value <= right_value )
		push_char( TRUE );

	push_char( FALSE );

	return MEM_OK;
}

int sol_string_compare( Operator op, Value* args )
{
	char* left_value = pop_string();
	char* right_value = pop_string();

	if( op == SOL_SGT && strcmp( left_value, right_value ) > 0 )
		push_char( TRUE );
	else if( op == SOL_SGE && strcmp( left_value, right_value ) >= 0 )
		push_char( TRUE );
	else if( op == SOL_SLT && strcmp( left_value, right_value ) < 0 )
		push_char( TRUE );
	else if( op == SOL_SLE && strcmp( left_value, right_value ) <= 0 )
		push_char( TRUE );

	push_char( FALSE );

	return MEM_OK;
}

// TODO implement
int sol_in( Value* args )
{
	return 0;
}

// BORING
int sol_int_math( Operator op, Value* args )
{
	int left_value = pop_int();
	int right_value = pop_int();

	int result;

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

int sol_real_math( Operator op, Value* args )
{
	float left_value = pop_real();
	float right_value = pop_real();

	float result;

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

// TODO implement
int sol_iumi( Value* args )
{
	return 0;
}

// TODO implement
int sol_rumi( Value* args )
{
	return 0;
}

// TODO implement
int sol_neg( Value* args )
{
	return 0;
}

// TODO implement
int sol_wr( Value* args )
{
	return 0;
}

// TODO implement
int sol_fwr( Value* args )
{
	return 0;
}

// TODO implement
int sol_push( Value* args )
{
	return 0;
}

// TODO implement
int sol_goto( Value* args )
{
	return 0;
}

// TODO implement
int sol_pop( Value* args )
{
	return 0;
}

// TODO implement
int sol_rd( Value* args )
{
	return 0;
}

// TODO implement
int sol_frd( Value* args )
{
	return 0;
}

// TODO implement
int sol_toint( Value* args )
{
	return 0;
}

// TODO implement
int sol_toreal( Value* args )
{
	return 0;
}

// TODO implement
int sol_read( Value* args )
{
	return 0;
}

// TODO implement
int sol_fread( Value* args )
{
	return 0;
}

// TODO implement
int sol_write( Value* args )
{
	return 0;
}

// TODO implement
int sol_fwrite( Value* args )
{
	return 0;
}

// TODO implement
int sol_func( Value* args )
{
	return 0;
}

// TODO implement
int sol_return( Value* args )
{
	return 0;
}

// TODO implement
int sol_scode( Value* args )
{
	return 0;
}

// TODO implement
int sol_halt( Value* args )
{
	return 0;
}

int sol_noop()
{
	return 0;
}

char* get_filename_extension( char* filename )
{
    char *dot = strrchr( filename, '.' );
    
	if( !dot || dot == filename )
		return "";

	return dot + 1;
}
