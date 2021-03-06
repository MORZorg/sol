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
#include "../libs/hashmap.h"

#define byte char

#define TRUE '1'
#define FALSE '0'

#define MEM_OK 0
#define ERROR_ALLOC 1
#define ERROR_ASTACK_OUT_OF_BOUND 2
#define ERROR_OSTACK_OUT_OF_BOUND 3
#define ERROR_ISTACK_OUT_OF_BOUND 4
#define ERROR_UNRECOGNISED 5

#define ASTACK_UNIT sizeof( Adescr* )
#define OSTACK_UNIT sizeof( Odescr* )
#define ISTACK_UNIT sizeof( byte )

typedef enum
{
	EMB,
	STA
} Mode;

// If the value is embedded, then it must have size < sizeof(byte)
typedef union
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
	int first_object;
	// Address were to return
	int raddr;
	// Address of the father in the astack
	int alink;
} Adescr;

typedef struct
{
	long size;
	byte* value;
} ByteArray;

// Position of the actual statement to execute
// Normally, the pc is increased by one after every instruction execution, but it can
// be modified completely, for example, to perform a jump in the code (the statements vector)
extern int pc;

// Pointers to first free positions in the stacks
extern int ap, op, ip;
// Actual allocated sizes of the stacks
extern int asize, osize, isize;

// The stacks (actually vectors)
extern Adescr** astack;
extern Odescr** ostack;
extern byte* istack;

int initialize_stacks( void );
int finalize_stacks( void );

// Interaction with the persistent stacks
byte top_istack( void );

void deallocate_istack( int );
void allocate_istack( int );

Odescr* top_ostack( void );

void pop_ostack( void );
void push_ostack( Odescr* );

// Allocate the number of objects defined in a function definition header
// to avoid multiple realloc
void enlarge_ostack( int );

Adescr* top_astack( void );

void pop_astack( void );
void push_astack( Adescr* );

// Embedded instructions to interact with the temporary istack
ByteArray pop_bytearray( void );
void push_bytearray( byte*, int );

int pop_int( void );
void push_int( int );

float pop_real( void );
void push_real( float );

char pop_char( void );
void push_char( char );

char* pop_string( void );
void push_string( char* );

// Conversions
char* adjust_bytearray( ByteArray*, ByteArray*, char*, char );
ByteArray encrypt_bytearray( ByteArray*, char* );
ByteArray decrypt_bytearray( ByteArray*, char* );
void encrypt_string( ByteArray*, ByteArray* );
void decrypt_string( ByteArray*, ByteArray* );

#endif
