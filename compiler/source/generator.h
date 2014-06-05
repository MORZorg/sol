#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "def.h"
#include "stacklist.h"

#define FILE_EXTENSION ".oHan"
#define NAME_FILE(a) a FILE_EXTENSION

int yygen();
Boolean code_generation( Node* );

int yygenerror( Node*, char* );

#endif // __GENERATOR_H__
