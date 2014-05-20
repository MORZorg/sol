#include "analyzer.h"

extern Node* root;

int yysem()
{
	// Passing through the syntax tree to check semantic
	return check_syntax_tree( root );
}

int check_syntax_tree( Node* node )
{
	int oid_relative = 1;
	int oid_absolute = 1;


	Symbol* element = create_symbol_table_element( node, oid_absolute );
	oid_absolute++;
	
	// Check if there is another func_decl and process it
	
	// Processing body of the function
}

Symbol* create_symbol_table_element( Node* node, int oid )
{
	Symbol* result = malloc( sizeof( Symbol ) );
	result->name = node->child->s_val;

	switch( node->type )
	{
		case N_TYPE_SECT:
			result->class = CLASS_TYPE;
			break;

		case N_VAR_SECT:
			result->class = CLASS_VAR;
			break;

		case N_CONST_DECL:
			result->class = CLASS_CONST;
			break;

		case N_FUNC_DECL:
			result->oid = oid;
			result->class = CLASS_FUNC;
			result->schema = look_into_schema_table( node->child->brother->brother );
			break;

		// PARAMETERS
	}

	
	return result;
}
