#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <string.h>
#include <stdarg.h>

#include "def.h"
#include "analyser.h"
#include "../libs/stacklist.h"

#define INTERMEDIATE_CODE_EXTENSION ".ohana"

#define ERROR_UNDEFINED_FLAG "Undefined flag."

#define MAX_INT_LEN 33
#define MAX_ARGS 3

typedef enum
{
	SOL_NEW,
	SOL_NEWS,
	SOL_LDC,
	SOL_LDI,
	SOL_LDR,
	SOL_LDS,
	SOL_LOD,
	SOL_CAT,
	SOL_LDA,
	SOL_FDA,
	SOL_EIL,
	SOL_SIL,
	SOL_IXA,
	SOL_STO,
	SOL_IST,
	SOL_JMF,
	SOL_JMP,
	SOL_EQU,
	SOL_NEQ,
	SOL_CGT,
	SOL_CGE,
	SOL_CLT,
	SOL_CLE,
	SOL_IGT,
	SOL_IGE,
	SOL_ILT,
	SOL_ILE,
	SOL_RGT,
	SOL_RGE,
	SOL_RLT,
	SOL_RLE,
	SOL_SGT,
	SOL_SGE,
	SOL_SLT,
	SOL_SLE,
	SOL_IN,
	SOL_IPLUS,
	SOL_IMINUS,
	SOL_ITIMES,
	SOL_IDIV,
	SOL_RPLUS,
	SOL_RMINUS,
	SOL_RTIMES,
	SOL_RDIV,
	SOL_IUMI,
	SOL_RUMI,
	SOL_NEG,
	SOL_WR,
	SOL_FWR,
	SOL_PUSH,
	SOL_GOTO,
	SOL_POP,
	SOL_RD,
	SOL_FRD,
	SOL_TOINT,
	SOL_TOREAL,
	SOL_READ,
	SOL_FREAD,
	SOL_WRITE,
	SOL_FWRITE,
	SOL_FUNC,
	SOL_RETURN,
	SOL_SCODE,
	SOL_HALT
} Operator;

typedef union
{
	int i_val;
	char* s_val;
	float r_val;
} Lexval;

typedef struct stat
{
	int address;
	Operator op;
	Lexval args[ MAX_ARGS ];
	struct stat* next;
} Stat;

typedef struct code
{
	Stat* head;
	int size;
	Stat* tail;
} Code;

typedef struct func_location
{
	int size; // # of objects in the activation record of this function
	int scope; // level containing the function declaration, used to calculate the chain in func_call
	int entry; // Function's entry
} FuncDesc;

int yygen( FILE*, FILE* );
Code generate_intro_code( Symbol* );
Code generate_code( Node* );
Code generate_lhs_code( Node*, Schema**, Boolean );

Code empty_code( void );
void relocate_address( Code, int );
Code append_code( Code, Code );
Code concatenate_code( int, Code, Code, ... );
Code make_code_no_param( Operator );
Code make_code_one_param( Operator, int );
Code make_code_two_param( Operator, int, int );
Code make_code_string_param( Operator, char* );
Code make_push_pop( int, int, int );
Code make_ldc( char* );
Code make_ldi( int );
Code make_ldr( float );
Code make_lds( char* );
Code make_decl( Schema* );

size_t schema_size( Schema* );
char* schema_to_string( Schema* );
void output_code( FILE*, Code );

map_t func_map; // Parameters needed by function call

#include "tree_print.h"

#endif // __GENERATOR_H__
