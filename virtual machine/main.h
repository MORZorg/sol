#ifndef __MAIN_H__
#define __MAIN_H__


#include "string.h"
#include "def.h"
#include "executor.h"

#define ERROR_UNDEFINED_FLAG "No such flag."
#define SOL_EXTENSION "sol"
#define OHANA_EXTENSION "ohana"
#define GENERATOR_PATH "./solc"

char* get_file_extension( char* );
char* change_file_extension( char*, char*, char* );
char* formatted_command( char* );
void print_help( void );

#endif // __MAIN_H__
