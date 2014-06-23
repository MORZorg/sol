%{
#include <string.h>

#include "def.h"

#define YYSTYPE Node*
extern FILE* yyin;
extern char* yytext;
extern Value lexval;
extern int line;

extern int yylex();
extern int yyerror( char* );

Node* root = NULL;
%}

%token OP INT_CONST CHAR_CONST REAL_CONST STR_CONST BOOL_CONST

%%

program : stat_list
		;

stat_list : stat stat_list
		  | 
		  ;

stat : no_param
	 | one_param
	 | string_param
	 | two_param
	 | read
	 | ldc
	 | ldr
	 ;

no_param : OP
		 ;

one_param : OP INT_CONST
		  ;

string_param : OP STR_CONST
			 ;

two_param : OP INT_CONST INT_CONST
		  ;

read : OP INT_CONST INT_CONST STR_CONST
	 ;

ldc : OP CHAR_CONST
	;

ldr : OP REAL_CONST
	;

%%

// Node* new_node( TypeNode type )
// {
// 	Node* result = malloc( sizeof( Node ) );
// 	result->type = type;
// 	result->child = result->brother = NULL;
// 	result->line = line;
// 
// 	return result;
// }
// 
// Node* new_nonterminal_node( Nonterminal value )
// {
// 	Node* result = new_node( T_UNQUALIFIED_NONTERMINAL );
// 	result->value.n_val = value;
// 
// 	return result;
// }
// 
// Node* new_terminal_node( TypeNode type, Value value )
// {
// 	Node* result = new_node( type );
// 	
// 	switch( type )
// 	{
// 		case T_INT_CONST:
// 			result->value.i_val = value.i_val;
// 			break;
// 
//         case T_CHAR_CONST:
//             result->value.c_val = value.c_val;
//             break;
// 
//         case T_REAL_CONST:
//             result->value.r_val = value.r_val;
//             break;
// 
//         case T_BOOL_CONST:
//             result->value.b_val = value.b_val;
//             break;
// 
//         case T_STR_CONST:
// 		case T_ID:
//         case T_ID_DOMAIN:
// 			result->value.s_val = value.s_val;
// 			break;
// 
// 		default:
// 			break;
// 	}
// 
// 	return result;
// }
// 
// Node* new_qualified_node( TypeNode type, Qualifier qualifier )
// {
// 	Node* result = new_node( type );
// 	result->value.q_val = qualifier;
// 
// 	return result;
// }
// 
// /**
//  * @brief Assigns to the given pointer to a tree node (Node*) the given tree
//  *        node or its last brother.
//  * @note Can be optimized if brother can't be NULL.
//  *
//  * @param initial
//  * @param brother
//  *
//  * @return A pointer to the given tree node's brother in case of success,
//  *	the given pointer to a tree node otherwise.
//  */
// Node** assign_brother( Node** initial, Node* brother )
// {
// 	while( *initial != NULL )
// 	{
// 		initial = &( (*initial)->brother );
// 	}
// 
// 	if( brother != NULL )
// 	{
// 		*initial = brother;
// 		return &( brother->brother );
// 	}
// 
// 	return initial;
// }
// 
// Node* get_last_brother( Node* node )
// {
//     Node* current_node = node;
//     while( current_node->brother != NULL )
//         current_node = current_node->brother;
// 
//     return current_node;
// }
// 
// void print_node( Node* node )
// {
// 	fprintf( stderr, "\n *** NODE *** \t" );
// 	fprintf( stderr, "Line: %d\t", node->line );
// 	fprintf( stderr, "TypeNode: %d\t\n", node->type );
// }
