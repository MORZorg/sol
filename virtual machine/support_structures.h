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

#define byte char

#define TRUE '1'
#define FALSE '0'

#define MEM_OK 0
#define ALLOC_ERROR 1
#define ASTACK_OUT_OF_BOUND 2
#define OSTACK_OUT_OF_BOUND 3
#define ISTACK_OUT_OF_BOUND 4

#define ASTACK_UNIT sizeof( Adescr* )
#define OSTACK_UNIT sizeof( Odescr* )
#define ISTACK_UNIT sizeof( byte )

typedef enum
{
	EMB,
	STA
} Mode;

// If the value is embedded, then it must have size < sizeof(byte)
typedef struct
{
	byte* emb_val;
	int sta_val; // istack offset of the complex value start
} ObjectVal;

typedef struct
{
	// Modality of saving of the object, either embedded or in the instance stack
	Mode mode;
	// Size of the object in bytes
	int size;
	// Value
	ObjectVal inst;
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
byte* istack;

int initialize_stacks();

// Interaction with the istack
byte top_istack();

void pop_istack();
void push_istack( byte );

byte* pop_bytearray();
void push_bytearray( byte*, int );

int pop_int();
void push_int( int );

float pop_real();
void push_real( float );

char pop_char();
void push_char( char );

char* pop_string();
void push_string( char* );

// Interactions with the other stacks
Odescr* top_ostack();

void pop_ostack();
void push_ostack( Odescr* );

Adescr* top_astack();

void pop_astack();
void push_astack( Adescr* );

#endif
