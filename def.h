#include <stdio.h>
#include <stdlib.h>

typedef union
{
	int i_val;
	char* s_val;
    char c_val;
	float r_val;
	enum { FALSE, TRUE } b_val;
} Value;

char* new_string( char* );
