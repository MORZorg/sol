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

// Takes as parameter a Stat* list generated by lex + yacc
int yyvm( Stat** );
int execute();

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
int sol_ist( Value* );
int sol_jmf( Value* );
int sol_jmp( Value* );
int sol_equ( Value* );
int sol_neq( Value* );
int sol_cgt( Value* );
int sol_cge( Value* );
int sol_clt( Value* );
int sol_cle( Value* );
int sol_igt( Value* );
int sol_ige( Value* );
int sol_ilt( Value* );
int sol_ile( Value* );
int sol_rgt( Value* );
int sol_rge( Value* );
int sol_rlt( Value* );
int sol_rle( Value* );
int sol_sgt( Value* );
int sol_sge( Value* );
int sol_slt( Value* );
int sol_sle( Value* );
int sol_in( Value* );
int sol_iplus( Value* );
int sol_iminus( Value* );
int sol_itimes( Value* );
int sol_idiv( Value* );
int sol_rplus( Value* );
int sol_rminus( Value* );
int sol_rtimes( Value* );
int sol_rdiv( Value* );
int sol_iumi( Value* );
int sol_rumi( Value* );
int sol_neg( Value* );
int sol_wr( Value* );
int sol_fwr( Value* );
int sol_push( Value* );
int sol_goto( Value* );
int sol_pop( Value* );
int sol_rd( Value* );
int sol_frd( Value* );
int sol_toint( Value* );
int sol_toreal( Value* );
int sol_read( Value* );
int sol_fread( Value* );
int sol_write( Value* );
int sol_fwrite( Value* );
int sol_func( Value* );
int sol_return( Value* );
int sol_scode( Value* );
int sol_halt( Value* );
int sol_noop();

char* get_filename_extension( char* );

#endif // EXECUTOR_H
