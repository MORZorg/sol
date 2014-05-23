#include "tree_print.h"

const char* TREE_NONTERMINALS[] =
{
	"FUNC_DECL",
	"PAR_LIST",
	"DECL",
	"STRUCT_DOMAIN",
	"VECTOR_DOMAIN",
	"TYPE_SECT",
	"VAR_SECT",
	"CONST_SECT",
	"CONST_DECL",
	"FUNC_LIST",
	"FUNC_BODY",
	"STAT_LIST",
	"ASSIGN_STAT",
	"FIELDING",
	"INDEXING",
	"IF_STAT",
	"ELSIF_STAT",
	"ELSE_STAT",
	"WHILE_STAT",
	"FOR_STAT",
	"FOREACH_STAT",
	"RETURN_STAT",
	"READ_STAT",
	"WRITE_STAT",
	"EXPR",
	"BOOL_TERM",
	"REL_TERM",
	"LOW_TERM",
    "ELSIF_EXPR_LIST",
	"FUNC_CALL",
	"COND_EXPR",
	"DYNAMIC_INPUT",
	"DYNAMIC_OUTPUT",
};

const char* TREE_TYPES[] =
{
	"ATOMIC_DOMAIN",
    "ID_DOMAIN",

	"LOGIC_EXPR",
	"REL_EXPR",
	"MATH_EXPR",
	"NEG_EXPR",
	"INSTANCE_EXPR",
	"BUILT_IN_CALL",

    "BREAK_STAT",

	"INT_CONST",
	"CHAR_CONST",
	"REAL_CONST",
	"STR_CONST",
	"BOOL_CONST",
    "ID",
    "UNQUALIFIED_NONTERMINAL"
};

const char* TREE_QUALIFIERS[] =
{
	"CHAR",
	"INT",
	"REAL",
	"STRING",
	"BOOL",

	"AND",
	"OR",

	"EQ",
	"NEQ",
	"GT",
	"GEQ",
	"LT",
	"LEQ",
	"IN",

	"NOT",

	"PLUS",
	"MINUS",
	"MULTIPLY",
	"DIVIDE",

	"TOINT",
	"TOREAL",

    "STRUCT",
    "VECTOR"
};

const char* SPACING = "  ";

void tree_print(Node* root, int indent)
{
	int i;
	Node* p;

	for( i = 0; i < indent; i++ )
		printf( "%s", SPACING );

	printf( "%s", ( root->type == T_UNQUALIFIED_NONTERMINAL ?
					TREE_NONTERMINALS[ root->value.n_val ] :
					TREE_TYPES[ root->type ] ) );

	switch( root->type )
	{
		case T_LOGIC_EXPR:
		case T_REL_EXPR:
		case T_MATH_EXPR:
		case T_NEG_EXPR:
		case T_INSTANCE_EXPR:
		case T_BUILT_IN_CALL:
		case T_ATOMIC_DOMAIN:
			printf( " (%s)", TREE_QUALIFIERS[ root->value.q_val ] );
			break;

		case T_INT_CONST:
			printf( " (%d)", root->value.i_val );
			break;

		case T_CHAR_CONST:
			printf( " (%c)", root->value.c_val );
			break;

		case T_REAL_CONST:
			printf( " (%f)", root->value.r_val );
			break;

		case T_BOOL_CONST:
			printf( " (%s)", (root->value.b_val == TRUE ? "true" : "false") );
			break;

		case T_STR_CONST:
		case T_ID:
        case T_ID_DOMAIN:
			printf( " (%s)", root->value.s_val );
			break;

		default:
			break;
	}
	printf( "\n" );

	for( p = root->child; p != NULL; p = p->brother )
		tree_print( p, indent + 1 );
}
