#include "generator.h"

extern FILE* yyin;
extern Node* root;
extern Symbol* symbol_table;
extern stacklist scope;

FILE* yyin;


int yygen( FILE* input, FILE* output )
{
	yyin = input;

	int sem_result = yysem();
	if( sem_result != 0 )
		return sem_result;

	stacklist_push( &scope, (stacklist_t) symbol_table );
	Code result = generate_code( root );

	// TODO Replace with something like `output_code'
	code_print( result );

	return 0;
}

Code generate_code( Node* node )
{
	Code result = empty_code();

	switch( node->type )
	{
		case T_INT_CONST:
			result = make_ldi( node->value.i_val );
			break;

		case T_CHAR_CONST:
			result = make_ldc( node->value.c_val );
			break;

		case T_REAL_CONST:
			result = make_ldr( node->value.r_val );
			break;

		case T_STR_CONST:
			result = make_lds( node->value.s_val );
			break;

		case T_BOOL_CONST:
			result = make_ldc( node->value.b_val == TRUE ? '1' : '0' );
			break;

		case T_ID:
		{
			Symbol* referenced_id = fetch_scope( node->value.s_val );
			result = make_code_two_param( SOL_LOD, referenced_id->nesting, referenced_id->oid );
			break;
		}

		case T_INSTANCE_EXPR:
		{
			Node* current_value = node->child;
			int size = 0;
			while( current_value != NULL )
			{
				result = append_code( result, generate_code( current_value ) );
				size++;

				current_value = current_value->brother;
			}

			result = append_code( result, make_code_two_param( SOL_CAT, size, schema_size( infere_expression_schema( node ) ) ) );
		}

		case T_UNQUALIFIED_NONTERMINAL:
			switch( node->value.n_val )
			{
				case N_FUNC_DECL:
				{
					Node* current_node = node->child;
					// TODO Find a way to avoid to duplicate the base function.
					stacklist_push( &scope, (stacklist_t) fetch_scope( current_node->value.s_val ) );
					current_node = current_node->brother;
					if( current_node->type == T_UNQUALIFIED_NONTERMINAL )
					{
						// TODO Params?
						current_node = current_node->brother;
					}
					current_node = current_node->brother;

					if( current_node->value.n_val == N_TYPE_SECT )
						current_node = current_node->brother;
					Code var_sect = empty_code();
					if( current_node->value.n_val == N_VAR_SECT )
					{
						var_sect = generate_code( current_node );
						current_node = current_node->brother;
					}
					Code const_sect = empty_code();
					if( current_node->value.n_val == N_CONST_SECT )
					{
						const_sect = generate_code( current_node );
						current_node = current_node->brother;
					}
					Code func_sect = empty_code();
					if( current_node->value.n_val == N_FUNC_LIST )
					{
						Node* current_child = current_node->child;
						do
						{
							func_sect = append_code( func_sect, generate_code( current_child ) );
							current_child = current_child->brother;
						} while( current_child != NULL );

						current_node = current_node->brother;
					}

					Code body = generate_code( current_node->child->brother );

					return concatenate_code( var_sect, const_sect, func_sect, body );
				}

				case N_VAR_SECT:
				{
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						Node* current_id = current_child->child;
						while( current_id->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );
							fprintf( stderr, "Var %s: %s.\n", current_id->value.s_val, schema_to_string( fetch_scope( current_id->value.s_val )->schema ) );

							current_id = current_id->brother;
						}

						current_child = current_child->brother;
					}
					break;
				}

				case N_CONST_SECT:
				{
					Code values = empty_code();
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						int const_size = 0;
						Node* current_id = current_child->child;
						while( current_id->brother->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );
							const_size++;
							fprintf( stderr, "Const %s: %s.\n", current_id->value.s_val, schema_to_string( fetch_scope( current_id->value.s_val )->schema ) );

							current_id = current_id->brother;
						}

						for( ; const_size > 0; const_size-- )
							values = append_code( values, generate_code( current_id->brother ) );

						current_child = current_child->brother;
					}

					result = append_code( result, values );
					break;
				}


				default:
					break;
			}

		default:
			break;
	}

	return result;
}

Code empty_code( void )
{
	static Code code = { NULL, 0, NULL };
	return code;
}

void relocate_address( Code code, int offset )
{
	if( offset == 0 )
		return;

	Stat* current_stat;
	for( current_stat = code.head;
		 current_stat != NULL;
		 current_stat = current_stat->next )
		current_stat->address += offset;
}

Code append_code( Code first, Code second )
{
	if( first.size == 0 )
		return second;
	if( second.size == 0 )
		return first;

	Code result;
	relocate_address( second, first.size );
	result.head = first.head;
	result.tail = second.tail;
	first.tail->next = second.head;
	result.size = first.size + second.size;

	return result;
}

Code concatenate_code( Code code1, Code code2, ... )
{
	Code result = code1;

	Code* current_code;
	for( current_code = &code2; current_code->head != NULL; current_code++ )
		result = append_code( result, *current_code );

	return result;
}

Stat* new_stat( Operator op )
{
	Stat* result = malloc( sizeof( Stat ) );
	result->address = 0;
	result->op = op;
	result->next = NULL;

	return result;
}

Code make_code_no_param( Operator op )
{
	Code result;
	result.head = result.tail = new_stat(op);
	result.size = 1;

	return result;
}

Code make_code_one_param( Operator op, int arg )
{
	Code result = make_code_no_param(op);
	result.head->args[ 0 ].i_val = arg;

	return result;
}

Code make_code_two_param( Operator op, int arg1, int arg2 )
{
	Code result = make_code_no_param(op);
	result.head->args[ 0 ].i_val = arg1;
	result.head->args[ 1 ].i_val = arg2;

	return result;
}

Code make_push_pop( int size, int chain, int entry )
{
	return concatenate_code( make_code_two_param( SOL_PUSH, size, chain ),
							 make_code_one_param( SOL_GOTO, entry ),
							 make_code_no_param( SOL_POP ) );
}

Code make_ldc( char a_char )
{
	Code result;
	result = make_code_no_param( SOL_LDC );
	result.head->args[ 0 ].c_val = a_char;

	return result;
}

Code make_ldi( int an_int )
{
	Code result;
	result = make_code_no_param( SOL_LDI );
	result.head->args[ 0 ].i_val = an_int;

	return result;
}

Code make_ldr( float a_real )
{
	Code result;
	result = make_code_no_param( SOL_LDR );
	result.head->args[ 0 ].r_val = a_real;

	return result;
}

Code make_lds( char* a_string )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
	result.head->args[ 0 ].s_val = a_string;

	return result;
}

Code make_decl( Schema* a_schema )
{
	Operator op;
	switch( a_schema->type )
	{
		case TS_VECTOR:
		case TS_STRUCT:
			op = SOL_NEWS;
			break;

		default:
			op = SOL_NEW;
			break;
	}

	return make_code_one_param( op, schema_size( a_schema ) );
}

size_t schema_size( Schema* a_schema )
{
	switch( a_schema->type )
	{
		case TS_CHAR:
			return sizeof( char );

		case TS_INT:
			return sizeof( int );

		case TS_REAL:
			return sizeof( float );

		case TS_STRING:
			return sizeof( char* );

		case TS_BOOL:
			return sizeof( char );

		case TS_STRUCT:
		{
			size_t size = 0;
			Schema* current_attr;
			for( current_attr = a_schema->child;
				 current_attr != NULL;
				 current_attr = current_attr->brother )
				size += schema_size( current_attr );

			return size;
		}

		case TS_VECTOR:
			return a_schema->size * schema_size( a_schema->child );

		case TS_ATTR:
			return schema_size( a_schema->child );
	}
}

char* schema_to_string( Schema* a_schema )
{
	switch( a_schema->type )
	{
		case TS_CHAR:
			return "c";

		case TS_INT:
			return "i";

		case TS_REAL:
			return "r";

		case TS_STRING:
			return "s";

		case TS_BOOL:
			return "b";

		case TS_STRUCT:
		{
			char* result = malloc( 2 * sizeof( char ) );
			result[ 0 ] = '(';

			Schema* current_attr;
			for( current_attr = a_schema->child;
				 current_attr != NULL;
				 current_attr = current_attr->brother )
			{
				char* schema = schema_to_string( current_attr->child );
				realloc( result, ( strlen( result ) + strlen( current_attr->id ) + strlen( schema ) ) * sizeof( char ) );
				sprintf( result, "%s%s:%s,", result, current_attr->id, schema );
			}
			result[ strlen( result )-1 ] = ')';
			return result;
		}

		case TS_VECTOR:
		{
			char* schema = schema_to_string( a_schema->child );
			char* result = malloc( ( MAX_INT_LEN + strlen( schema ) + 3 * sizeof( char ) ) );
			sprintf( result, "[%d,%s]", a_schema->size, schema );
			return result;
		}

		case TS_ATTR:
			return schema_to_string( a_schema->child );
	}
}

/**
 * @brief Error function called in case of error while generating code.
 *
 * @param node
 * @param type
 *
 * @return
 */
int yygenerror( Node* node, char* type )
{
	return 0;
}
