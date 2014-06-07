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
    "ELSIF_EXPR",
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

const char* TABLE_CLASSES[] =
{
	"TYPE",
	"VAR",
	"CONST",
	"FUNC",
	"PAR"
};

const char* TABLE_TYPES[] =
{
	"CHAR",
	"INT",
	"REAL",
	"STRING",
	"BOOL",
	"STRUCT",
	"VECTOR",
	"ATTR"
};

const char* CODE_OPERATORS[] =
{
  "NEW",
  "NEWS",
  "LDC",
  "LDI",
  "LDR",
  "LDS",
  "LOD",
  "CAT",
  "LDA",
  "FDA",
  "EIL",
  "SIL",
  "IXA",
  "STO",
  "IST",
  "JMF",
  "JMP",
  "EQU",
  "NEQ",
  "CGT",
  "CGE",
  "CLT",
  "CLE",
  "IGT",
  "IGE",
  "ILT",
  "ILE",
  "RGT",
  "RGE",
  "RLT",
  "RLE",
  "SGT",
  "SGE",
  "SLT",
  "SLE",
  "IN",
  "IPLUS",
  "IMINUS",
  "ITIMES",
  "IDIV",
  "RPLUS",
  "RMINUS",
  "RTIMES",
  "RDIV",
  "IUMI",
  "RUMI",
  "NEG",
  "WE",
  "FWR",
  "PUSH",
  "GOTO",
  "POP",
  "RD",
  "FRD",
  "TOINT",
  "TOREA",
  "READ",
  "FREAD",
  "WRITE",
  "FWRITE",
  "FUNC",
  "RETURN",
};

const char* SPACING = "  ";

void print_indent( int indent )
{
	int i;
	for( i = 0; i < indent; i++ )
		printf( "%s", SPACING );
}

void tree_print( Node* root, int indent )
{
	Node* p;

    print_indent( indent );

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

int table_print_hashmap( any_t indent, any_t root )
{
	table_print( (Symbol*) root, *(int*) indent );
	return MAP_OK;
}

void table_print( Symbol* root, int indent )
{
	int i;

    print_indent( indent );
    printf( "%-6s %-4d %-10s %-3d ", TABLE_CLASSES[ root->clazz ], root->oid, root->name, root->nesting );
	schema_print( root->schema );
	printf( "\n" );

	if( root->clazz == CS_FUNC )
		for( i = 0; i < root->formals_size; i++ )
			table_print( root->formals[ i ], indent + 2 );

	int next_indent = indent + 1;
	hashmap_iterate( root->locenv, table_print_hashmap, (any_t) &next_indent );
}

void schema_print( Schema* root )
{
    printf( "%s", TABLE_TYPES[ root->type ] );
	if( root->id != NULL )
		printf( ": %s ", root->id );
	if( root->size > 0 )
		printf( "[%d]", root->size );

	if( root->child != NULL )
	{
		printf( "( " );
		schema_print( root->child );
		printf( " )" );
	}
	if( root->brother != NULL )
	{
		printf( ", " );
		schema_print( root->brother );
	}
}

void code_print( Code code )
{
	printf( "Size: %d.\n", code.size );
	Stat* current_stat;
	for( current_stat = code.head; current_stat != NULL; current_stat = current_stat->next )
		switch( current_stat->op )
		{
			case SOL_LDC:
				printf( "%d: %s %c\n",
						current_stat->address,
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].c_val );
				break;

			case SOL_LDS:
				printf( "%d: %s %s\n",
						current_stat->address,
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].s_val );
				break;

			case SOL_LDR:
				printf( "%d: %s %f\n",
						current_stat->address,
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].r_val );
				break;

			case SOL_LOD:
			case SOL_CAT:
				printf( "%d: %s %d %d\n",
						current_stat->address,
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].i_val,
						current_stat->args[ 1 ].i_val );
				break;

			default:
				printf( "%d: %s %d\n",
						current_stat->address,
						CODE_OPERATORS[ current_stat->op ],
						current_stat->args[ 0 ].i_val );
				break;
		}
}
