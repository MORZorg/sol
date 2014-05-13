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
	N_STRUCT_DOMAIN,
	N_VECTOR_DOMAIN,
	N_TYPE_SECT,
	N_VAR_SECT,
	N_CONST_SECT,
	N_CONST_DECL,
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
	T_ATOMIC_DOMAIN,
	T_TYPE,
	T_VAR,
	T_CONST,

	T_BEGIN,
	T_END,
	T_IF,
	T_THEN,
	T_ENDIF,
	T_ELSIF,
	T_ELSE,
	T_WHILE,
	T_DO,
	T_ENDWHILE,
	T_FOR,
	T_TO,
	T_ENDFOR,
	T_FOREACH,
	T_ENDFOREACH,

	T_RETURN,
	T_READ,
	T_WRITE,
	T_RD,
	T_WR,
	T_DEFINE,
	T_ASSIGN,

	T_AND,
	T_OR,
	T_IN,
	T_NOT,
	T_TOINT,
	T_TOREAL,
	T_EQ,
	T_NEQ,
	T_GT,
	T_GEQ,
	T_LT,
	T_LEQ,
	T_PLUS,
	T_MINUS,
	T_MULTIPLY,
	T_DIVIDE,

	T_INT_CONST,
	T_CHAR_CONST,
	T_REAL_CONST,
	T_STR_CONST,
	T_BOOL_CONST,
    T_ID,
    T_NONTERMINAL
} TypeNode;

typedef enum
{
	Q_CHAR,
	Q_INT,
	Q_REAL,
	Q_STRING,
	Q_BOOL
} Qualifier;

typedef union
{
	int i_val;
    char c_val;
	char* s_val;
	double r_val;
	Boolean b_val;
	Qualifier q_val;
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
Node* new_qualified_node( TypeNode, Qualifier );

Node** assign_brother( Node**, Node* );

#endif
