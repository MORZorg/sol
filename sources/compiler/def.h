#ifndef __DEF_H__
#define __DEF_H__

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	FALSE,
	TRUE
} Boolean;

typedef enum 
{
	N_FUNC_DECL,
	N_PAR_LIST,
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
	N_ASSIGN_STAT,
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
	N_WRITE_STAT,
	N_EXPR,
	N_BOOL_TERM,
	N_REL_TERM,
	N_LOW_TERM,
	N_ELSIF_EXPR,
	N_FUNC_CALL,
	N_COND_EXPR,
	N_DYNAMIC_INPUT,
	N_DYNAMIC_OUTPUT,
} Nonterminal;

typedef enum
{
	T_ATOMIC_DOMAIN,
	T_ID_DOMAIN,

	T_LOGIC_EXPR,
	T_REL_EXPR,
	T_MATH_EXPR,
	T_NEG_EXPR,
	T_INSTANCE_EXPR,
	T_BUILT_IN_CALL,

	T_BREAK_STAT,

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
	char* s_val;
	double r_val;
	Boolean b_val;
	Qualifier q_val;
	Nonterminal n_val;
} Value;

typedef struct snode
{
	int line;
	Value value;
	TypeNode type;
	struct snode* child;
	struct snode* brother;
} Node;

char* new_string( char* );
void concatenate_string( char**, char* );
char parse_escape_seq( char* );

Node* new_nonterminal_node( Nonterminal );
Node* new_terminal_node( TypeNode, Value );
Node* new_qualified_node( TypeNode, Qualifier );

Node** assign_brother( Node**, Node* );
Node* get_last_brother( Node* );
void print_node( Node* );

#endif
