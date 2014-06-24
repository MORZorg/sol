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
			sol_cgt( current_statement->args );
			break;

		case SOL_CGE:
			sol_cge( current_statement->args );
			break;

		case SOL_CLT:
			sol_clt( current_statement->args );
			break;

		case SOL_CLE:
			sol_cle( current_statement->args );
			break;

		case SOL_IGT:
			sol_igt( current_statement->args );
			break;

		case SOL_IGE:
			sol_ige( current_statement->args );
			break;

		case SOL_ILT:
			sol_ilt( current_statement->args );
			break;

		case SOL_ILE:
			sol_ile( current_statement->args );
			break;

		case SOL_RGT:
			sol_rgt( current_statement->args );
			break;

		case SOL_RGE:
			sol_rge( current_statement->args );
			break;

		case SOL_RLT:
			sol_rlt( current_statement->args );
			break;

		case SOL_RLE:
			sol_rle( current_statement->args );
			break;

		case SOL_SGT:
			sol_sgt( current_statement->args );
			break;

		case SOL_SGE:
			sol_sge( current_statement->args );
			break;

		case SOL_SLT:
			sol_slt( current_statement->args );
			break;

		case SOL_SLE:
			sol_sle( current_statement->args );
			break;

		case SOL_IN:
			sol_in( current_statement->args );
			break;

		case SOL_IPLUS:
			sol_iplus( current_statement->args );
			break;

		case SOL_IMINUS:
			sol_iminus( current_statement->args );
			break;

		case SOL_ITIMES:
			sol_itimes( current_statement->args );
			break;

		case SOL_IDIV:
			sol_idiv( current_statement->args );
			break;

		case SOL_RPLUS:
			sol_rplus( current_statement->args );
			break;

		case SOL_RMINUS:
			sol_rminus( current_statement->args );
			break;

		case SOL_RTIMES:
			sol_rtimes( current_statement->args );
			break;

		case SOL_RDIV:
			sol_rdiv( current_statement->args );
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

// TODO implement
// Not clear: it creates a new complex object with the last element_number elements on the stack, but where does the object go?
int sol_cat( Value* args )
{
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

	Odescr* object = &( astack[ ap - 1 - env_offset ]->objects[ oid ] );

	push_int( object->inst.sta_val );

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

// TODO implement
int sol_ixa( Value* args )
{
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

	object->inst.emb_val = pop_bytearray( object->size );

	return MEM_OK;
}

// TODO implement
int sol_ist( Value* args )
{
	return 0;
}

// TODO implement
int sol_jmf( Value* args )
{
	return 0;
}

// TODO implement
int sol_jmp( Value* args )
{
	return 0;
}

// TODO implement
int sol_equ( Value* args )
{
	return 0;
}

// TODO implement
int sol_neq( Value* args )
{
	return 0;
}

// TODO implement
int sol_cgt( Value* args )
{
	return 0;
}

// TODO implement
int sol_cge( Value* args )
{
	return 0;
}

// TODO implement
int sol_clt( Value* args )
{
	return 0;
}

// TODO implement
int sol_cle( Value* args )
{
	return 0;
}

// TODO implement
int sol_igt( Value* args )
{
	return 0;
}

// TODO implement
int sol_ige( Value* args )
{
	return 0;
}

// TODO implement
int sol_ilt( Value* args )
{
	return 0;
}

// TODO implement
int sol_ile( Value* args )
{
	return 0;
}

// TODO implement
int sol_rgt( Value* args )
{
	return 0;
}

// TODO implement
int sol_rge( Value* args )
{
	return 0;
}

// TODO implement
int sol_rlt( Value* args )
{
	return 0;
}

// TODO implement
int sol_rle( Value* args )
{
	return 0;
}

// TODO implement
int sol_sgt( Value* args )
{
	return 0;
}

// TODO implement
int sol_sge( Value* args )
{
	return 0;
}

// TODO implement
int sol_slt( Value* args )
{
	return 0;
}

// TODO implement
int sol_sle( Value* args )
{
	return 0;
}

// TODO implement
int sol_in( Value* args )
{
	return 0;
}

// TODO implement
int sol_iplus( Value* args )
{
	return 0;
}

// TODO implement
int sol_iminus( Value* args )
{
	return 0;
}

// TODO implement
int sol_itimes( Value* args )
{
	return 0;
}

// TODO implement
int sol_idiv( Value* args )
{
	return 0;
}

// TODO implement
int sol_rplus( Value* args )
{
	return 0;
}

// TODO implement
int sol_rminus( Value* args )
{
	return 0;
}

// TODO implement
int sol_rtimes( Value* args )
{
	return 0;
}

// TODO implement
int sol_rdiv( Value* args )
{
	return 0;
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
