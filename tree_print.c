#include "tree_print.h"

const char* TREE_TYPES[] =
{
	"INT_CONST",
	"CHAR_CONST",
	"REAL_CONST",
	"BOOL_CONST",
	"STR_CONST", 
	"ID",
	"NONTERMINAL"
};

const char* TREE_NONTERMINALS[] =
{
	"FUNC_DECL",
	"DECL_LIST",
	"DECL",
	"ID_LIST"
};

void tree_print(Node* root, int indent)
{
	int i;
	Node* p;

	for( i = 0; i < indent; i++ )
		printf("    ");

	printf( "%s", ( root->type == T_NONTERMINAL ?
					TREE_NONTERMINALS[ root->value.n_val ] :
					TREE_TYPES[ root->type ] ) );

	if ( root->type != T_NONTERMINAL )
		switch( root->type )
		{
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
				printf( " (%s)", root->value.s_val );
				break;

			case T_NONTERMINAL:
				// Just to avoid warnings.
				break;
		}
	printf( "\n" );

	for( p = root->child; p != NULL; p = p->brother )
		tree_print( p, indent + 1 );
}
