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
    // TODO Minimize me!
    
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
	N_ELSIF_STAT,
	N_ELSE_STAT,
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
	N_EXPR_LIST,
    N_ELSIF_EXPR_LIST,
	N_FUNC_CALL,
	N_EXPR_LIST_OPT,
	N_COND_EXPR,
	N_DYNAMIC_INPUT,
	N_DYNAMIC_OUTPUT,
	N_RETURN,
	N_READ,
	N_WRITE,
	N_RD,
	N_WR
} Nonterminal;

typedef enum
{
	T_ATOMIC_DOMAIN,
    T_ID_DOMAIN,
	T_TYPE,
	T_VAR,
	T_CONST,

	T_LOGIC_EXPR,
	T_REL_EXPR,
	T_MATH_EXPR,
	T_NEG_EXPR,
	T_INSTANCE_EXPR,
	T_BUILT_IN_CALL,

	T_INT_CONST,
	T_CHAR_CONST,
	T_REAL_CONST,
	T_STR_CONST,
	T_BOOL_CONST,
    T_ID,
    T_UNQUALIFIED_NONTERMINAL
} TypeNode;

typedef enum
{
	Q_CHAR,
	Q_INT,
	Q_REAL,
	Q_STRING,
	Q_BOOL,

	Q_AND,
	Q_OR,

	Q_EQ,
	Q_NEQ,
	Q_GT,
	Q_GEQ,
	Q_LT,
	Q_LEQ,
	Q_IN,

	Q_NOT,

	Q_PLUS,
	Q_MINUS,
	Q_MULTIPLY,
	Q_DIVIDE,

	Q_TOINT,
	Q_TOREAL,

    Q_STRUCT,
    Q_VECTOR
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
void concatenate_string( char*, char* );
char parse_escape_seq( char* );

Node* new_node( TypeNode );
Node* new_nonterminal_node( Nonterminal );
Node* new_terminal_node( TypeNode, Value );
Node* new_qualified_node( TypeNode, Qualifier );

Node** assign_brother( Node**, Node* );

#endif
