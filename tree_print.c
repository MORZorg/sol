#include "tree_print.h"

const char* TREE_NONTERMINALS[] =
{
	"FUNC_DECL",
	"DECL_LIST",
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
	"STAT",
	"ASSIGN_STAT",
	"LEFT_HAND_SIDE",
	"FIELDING",
	"INDEXING",
	"IF_STAT",
	"ELSIF_STAT_LIST_OPT",
	"ELSE_STAT_OPT",
	"WHILE_STAT",
	"FOR_STAT",
	"FOREACH_STAT",
	"RETURN_STAT",
	"READ_STAT",
	"SPECIFIER_OPT",
	"WRITE_STAT",
	"EXPR",
	"BOOL_OP",
	"BOOL_TERM",
	"REL_OP",
	"REL_TERM",
	"LOW_BIN_OP",
	"LOW_TERM",
	"HIGH_BIN_OP",
	"FACTOR",
	"UNARY_OP",
	"ATOMIC_CONST",
	"INSTANCE_CONSTRUCTION",
	"STRUCT_CONSTRUCTION",
	"EXPR_LIST",
	"VECTOR_CONSTRUCTION",
	"FUNC_CALL",
	"EXPR_LIST_OPT",
	"COND_EXPR",
	"ELSIF_EXPR_LIST_OPT",
	"BUILT_IN_CALL",
	"TOINT_CALL",
	"TOREAL_CALL",
	"DYNAMIC_INPUT",
	"DYNAMIC_OUTPUT",
	"RETURN",
	"READ",
	"WRITE",
	"RD",
	"WR"
};

const char* TREE_TYPES[] =
{
	"ATOMIC_DOMAIN",
    "ID_DOMAIN",
	"TYPE",
	"VAR",
	"CONST",

	"AND",
	"OR",
	"IN",
	"NOT",
	"EQ",
	"NEQ",
	"GT",
	"GEQ",
	"LT",
	"LEQ",
	"PLUS",
	"MINUS",
	"MULTIPLY",
	"DIVIDE",

	"INT_CONST",
	"CHAR_CONST",
	"REAL_CONST",
	"STR_CONST",
	"BOOL_CONST",
    "ID",
    "NONTERMINAL"
};

const char* TREE_QUALIFIERS[] =
{
	"CHAR",
	"INT",
	"REAL",
	"STRING",
	"BOOL"
};

const char* SPACING = "  ";

void tree_print(Node* root, int indent)
{
	int i;
	Node* p;

	for( i = 0; i < indent; i++ )
		printf( "%s", SPACING );

	printf( "%s", ( root->type == T_NONTERMINAL ?
					TREE_NONTERMINALS[ root->value.n_val ] :
					TREE_TYPES[ root->type ] ) );

	switch( root->type )
	{
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
