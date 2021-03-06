/*
 * File: executor.h
 * Description: Most methods of the virtual machine - header file
 * Date: 140623
 * Author: Mattia Rizzini
 * Group: ORZ
 */
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support_structures.h"
#include "dataIO.h"
#include "tree_print.h"

#define STA_NOT_ALLOCATED -1

int yyvm( int );
int execute( Stat );

// Execution of S-code instructions
int sol_new( Value* );
int sol_news( Value* );
int sol_ldc( Value* );
int sol_ldi( Value* );
int sol_ldr( Value* );
int sol_lds( Value* );
int sol_lod( Value* );
int sol_cat( Value* );
int sol_lda( Value* );
int sol_fda( Value* );
int sol_eil( Value* );
int sol_sil( Value* );
int sol_ixa( Value* );
int sol_sto( Value* );
int sol_ist( void );
int sol_jmf( Value* );
int sol_jmp( Value* );
int sol_equ( void );
int sol_neq( void );
int sol_char_compare( Operator );
int sol_int_compare( Operator );
int sol_real_compare( Operator );
int sol_string_compare( Operator );
int sol_in( void );
int sol_int_math( Operator );
int sol_real_math( Operator );
int sol_iumi( void );
int sol_rumi( void );
int sol_neg( void );
int sol_wr( Value* );
int sol_fwr( Value* );
int sol_push( Value* );
int sol_goto( Value* );
int sol_pop( void );
int sol_rd( Value* );
int sol_frd( Value* );
int sol_toint( void );
int sol_toreal( void );
int sol_read( Value* );
int sol_fread( Value* );
int sol_write( Value* );
int sol_fwrite( Value* );
int sol_func( Value* );
int sol_return( void );
int sol_noop( void );

#endif // EXECUTOR_H
