/*
 * File: support_structures.h
 * Description: VM stacks and methods to access them - header file
 * Date: 140624
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#ifndef SUPPORT_STRUCTURES_H
#define SUPPORT_STRUCTURES_H

#include <string.h>

#include "def.h"

#define MEM_OK 0
#define ALLOC_ERROR 1

#define ASTACK_UNIT sizeof( Adescr* )
#define OSTACK_UNIT sizeof( Odescr* )
#define ISTACK_UNIT sizeof( char )

typedef enum
{
	EMB,
	STA
} Mode;

typedef struct
{
	// Modality of saving of the object, either embedded or in the instance stack
	Mode mode;
	// Size of the object in bytes
	int size;
	// Value, either the complete value (i_val, s_val..) or a pointer to the right position in the instance_stack (char**?)
	Value inst;
	// oid of the object
	int oid;
} Odescr;

typedef struct
{
	// Number of objects contained in the activation record
	int obj_number;
	// Pointer to the first object of the activation record in object_stack
	Odescr* objects;
	// Address of the return instruction of the actual activation entry
	int raddr;
} Adescr;

// Position of the actual statement to execute
// Normally, the pc is increased by one after every instruction execution, but it can
// be modified completely, for example, to perform a jump in the code (the statements vector)
int pc;

// Pointers to first free positions in the stacks
int ap, op, ip;
// Actual allocated sizes of the stacks
int asize, osize, isize;

// The stacks (actually vectors)
Adescr** astack;
Odescr** ostack;
char* istack;

int initialize_stacks();

// Interaction with the istack
char top_istack();

void pop_istack();
void push_istack( char );

int pop_int();
void push_int( int );

float pop_real();
void push_real( float );

char pop_char();
void push_char( char );

char* pop_string( int );
void push_string( char* );

// Interactions with the other stacks
Odescr* top_ostack();

void pop_ostack();
void push_ostack( Odescr* );

Adescr* top_astack();

void pop_astack();
void push_astack( Adescr* );

#endif
