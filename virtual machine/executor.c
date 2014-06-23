/*
 * File: executor.c
 * Description: Most methods of the virtual machine
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#include "executor.h"

// Position of the actual statement to execute
// Normally, the pc is increased by one after every instruction execution, but it can
// be modified completely, for example, to perform a jump in the code (the statements vector)
int pc = 0;

stacklist astack,	// Stack of Adescr*
		  ostack,	// Stack of Odescr*
		  istack;	// Stack of char*

int yyvm( Stat** statements )
{
	ap = (Adescr*) astack;
	op = (Odescr*) ostack;
	ip = (char*) istack;

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

// Interaction with the istack
int pop_int()
{
	int value = (int) istack->function;
	stacklist_pop( &istack );

	return value;
}

void push_int( int value )
{
	stacklist_push( &istack, (stacklist_t) ( (char*) &value ) );
}

float pop_real()
{
	char object[ sizeof(float) ];

	for( int i = 0; i < sizeof(float); i++ )
	{
		object[i] = (char) istack->function;
		stacklist_pop( &istack );
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
		stacklist_push( &istack, (stacklist_t) &object[i] );
}

char pop_char()
{
	char value = (char) istack->function;
	stacklist_pop( &istack );

	return value;
}

void push_char( char value )
{
	stacklist_push( &istack, (stacklist_t) ( (char*) &value ) );
}

char* pop_string( int size )
{
	char* value = malloc( sizeof(char) * size );

	for( int i = 0; i < size; i++ )
	{
		value[i] = (char) istack->function;
		stacklist_pop( &istack );
	}

	return value;
}

void push_string( char* object )
{
	for( int i = 0; i < strlen( object ); i++ )
		stacklist_push( &istack, (stacklist_t) &object[i] );
}

// Execution of S-code instructions
int sol_new( Value* args )
{
	int size = args[0].i_val;

	Odescr* object = malloc( sizeof( Odescr ) );
	object->mode = EMB;
	object->size = size;

	stacklist_push( &ostack, (stacklist_t) object );

	return 0;
}

// TODO implement
int sol_news( Value* args )
{
	return 0;
}

int sol_ldc( Value* args )
{
	push_char( args[0].c_val );

	return 0;
}

int sol_ldi( Value* args )
{
	push_int( args[0].i_val );

	return 0;
}

int sol_ldr( Value* args )
{
	push_real( args[0].r_val );

	return 0;
}

int sol_lds( Value* args )
{
	push_string( args[0].s_val );

	return 0;
}

// TODO implement
int sol_lod( Value* args )
{
	int env_offset = args[0].i_val;
	int oid = args[0].i_val;

	// We go only by byte size, don't care about type

	return 0;
}

// TODO implement
int sol_cat( Value* args )
{
	return 0;
}

// TODO implement
int sol_lda( Value* args )
{
	return 0;
}

// TODO implement
int sol_fda( Value* args )
{
	return 0;
}

// TODO implement
int sol_eil( Value* args )
{
	return 0;
}

// TODO implement
int sol_sil( Value* args )
{
	return 0;
}

// TODO implement
int sol_ixa( Value* args )
{
	return 0;
}

// TODO implement
int sol_sto( Value* args )
{
	return 0;
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
