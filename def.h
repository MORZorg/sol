#include <stdio.h>
#include <stdlib.h>

#ifndef __DEF_H__
#define __DEF_H__

typedef enum
{
	FALSE,
	TRUE
} Boolean;

typedef enum 
{
	N_FUNC_DECL,
	N_DECL_LIST,
	N_DECL,
	N_ID_LIST,
	N_DOMAIN,
	N_ATOMIC_DOMAIN,
	N_STRUCT_DOMAIN,
	N_VECTOR_DOMAIN,
	N_TYPE_SECT_OPT,
	N_VAR_SECT_OPT,
	N_CONST_SECT_OPT,
	N_CONST_LIST,
	N_CONST_DECL,
	N_FUNC_LIST_OPT,
	N_FUNC_LIST,
	N_FUNC_BODY,
	N_STAT_LIST,
	N_STAT,
	N_ASSIGN_STAT,
	N_LEFT_HAND_SIDE,
	N_FIELDING,
	N_INDEXING,
	N_IF_STAT,
	N_ELSIF_STAT_LIST_OPT,
	N_ELSE_STAT_OPT,
	N_WHILE_STAT,
	N_FOR_STAT,
	N_FOREACH_STAT,
	N_RETURN_STAT,
	N_READ_STAT,
	N_SPECIFIER_OPT,
	N_WRITE_STAT,
	N_EXPR,
	N_BOOL_OP,
	N_BOOL_TERM,
	N_REL_OP,
	N_REL_TERM,
	N_LOW_BIN_OP,
	N_LOW_TERM,
	N_HIGH_BIN_OP,
	N_FACTOR,
	N_UNARY_OP,
	N_ATOMIC_CONST,
	N_INSTANCE_CONSTRUCTION,
	N_STRUCT_CONSTRUCTION,
	N_EXPR_LIST,
	N_VECTOR_CONSTRUCTION,
	N_FUNC_CALL,
	N_EXPR_LIST_OPT,
	N_COND_EXPR,
	N_ELSIF_EXPR_LIST_OPT,
	N_BUILT_IN_CALL,
	N_TOINT_CALL,
	N_TOREAL_CALL,
	N_DYNAMIC_INPUT,
	N_DYNAMIC_OUTPUT,
} Nonterminal;

typedef enum
{
	// TODO
/* %token FUNC CHAR INT REAL STRING BOOL STRUCT VECTOR OF TYPE VAR CONST */
/* %token SOL_BEGIN END IF THEN ENDIF ELSIF ELSE WHILE DO ENDWHILE FOR TO ENDFOR FOREACH ENDFOREACH */
/* %token RETURN READ WRITE RD WR DEFINE ASSIGN */
/* %token AND OR IN NOT TOINT TOREAL EQ NEQ GT GEQ LT LEQ PLUS MINUS MULTIPLY DIVIDE ID */
	T_INT_CONST,
	T_CHAR_CONST,
	T_REAL_CONST,
	T_STR_CONST,
	T_BOOL_CONST,
    T_ID,
    T_NONTERMINAL
} TypeNode;

typedef union
{
	int i_val;
    char c_val;
	char* s_val;
	double r_val;
	Boolean b_val;
	Nonterminal n_val;
} Value;

typedef struct snode
{
    TypeNode type;
    Value value;
    struct snode *child, *brother;
} Node;

char* new_string( char* );

Node* new_node( TypeNode );
Node* new_nonterminal_node( Nonterminal );
Node* new_terminal_node( TypeNode, Value );

Node** assign_brother( Node**, Node* );

#endif
