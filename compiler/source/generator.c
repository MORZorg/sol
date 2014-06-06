#include "generator.h"

extern FILE* yyin;
extern Node* root;
extern Symbol* symbol_table;

FILE* yyin;

stacklist scope;

int yygen( FILE* input, FILE* output )
{
	yyin = input;

	int sem_result = yysem();
	if( sem_result != 0 )
		return sem_result;

	Code result = code_generation( root );

	return 0;
}

Code code_generation( Node* node )
{
	Code result;

	switch( node->type )
	{
		case T_UNQUALIFIED_NONTERMINAL:
			switch( node->value.n_val )
			{
				case N_VAR_SECT:
				{
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						Node* current_id = current_child->child;
						while( current_id->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );

							current_id = current_id->brother;
						}
					}
					break;
				}

				case N_CONST_SECT:
				{
					Code values;
					Node* current_child = node->child;
					while( current_child != NULL )
					{
						int const_size = 0;
						Node* current_id = current_child->child;
						while( current_id->brother->brother != NULL )
						{
							result = append_code( result, make_decl( fetch_scope( current_id->value.s_val )->schema ) );
							const_size++;

							current_id = current_id->brother;
						}

						for( ; const_size > 0; const_size-- )
							values = append_code( values, code_generation( current_id->brother ) );
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

Code end_code()
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
	// TODO Why end_code()?
	return concatenate_code( make_code_two_param( SOL_PUSH, size, chain ),
							 make_code_one_param( SOL_GOTO, entry ),
							 make_code_no_param( SOL_POP ),
							 end_code() );
}

Code make_ldc( char a_char )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
	result.head->args[ 0 ].c_val = a_char;

	return result;
}

Code make_ldi( int an_int )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
	result.head->args[ 0 ].i_val = an_int;

	return result;
}

Code make_ldr( float a_real )
{
	Code result;
	result = make_code_no_param( SOL_LDS );
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

int schema_size( Schema* a_schema )
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
			return sizeof( char* );

		case TS_STRUCT:
		{
			int size = 0;
			Schema* current_attr;
			for( current_attr = a_schema ->child;
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
