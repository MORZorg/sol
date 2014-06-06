#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "def.h"
#include "stacklist.h"
#include "analyser.h"

#define INTERMEDIATE_CODE_EXTENSION ".ohana"

#define ERROR_UNDEFINED_FLAG "Undefined flag."

#define MAX_ARGS 3

typedef enum
{
  SOL_NEW,
  SOL_NEWS,
  SOL_LDC,
  SOL_LDI,
  SOL_LDR,
  SOL_LDS,
  SOL_LOD,
  SOL_CAT,
  SOL_LDA,
  SOL_FDA,
  SOL_EIL,
  SOL_SIL,
  SOL_IXA,
  SOL_STO,
  SOL_IST,
  SOL_JMF,
  SOL_JMP,
  SOL_EQU,
  SOL_NEQ,
  SOL_CGT,
  SOL_CGE,
  SOL_CLT,
  SOL_CLE,
  SOL_IGT,
  SOL_IGE,
  SOL_ILT,
  SOL_ILE,
  SOL_RGT,
  SOL_RGE,
  SOL_RLT,
  SOL_RLE,
  SOL_SGT,
  SOL_SGE,
  SOL_SLT,
  SOL_SLE,
  SOL_IN,
  SOL_IPLUS,
  SOL_IMINUS,
  SOL_ITIMES,
  SOL_IDIV,
  SOL_RPLUS,
  SOL_RMINUS,
  SOL_RTIMES,
  SOL_RDIV,
  SOL_IUMI,
  SOL_RUMI,
  SOL_NEG,
  SOL_WE,
  SOL_FWR,
  SOL_PUSH,
  SOL_GOTO,
  SOL_POP,
  SOL_RD,
  SOL_FRD,
  SOL_TOINT,
  SOL_TOREA,
  SOL_READ,
  SOL_FREAD,
  SOL_WRITE,
  SOL_FWRITE,
  SOL_FUNC,
  SOL_RETURN 
} Operator;

typedef union
{
  int i_val;
  char c_val;
  char* s_val;
  float r_val;
} Lexval;

typedef struct stat
{
  int addr;
  Operator op;
  Lexval args[ MAX_ARGS ];
  struct stat* next;
} Stat;

typedef struct code
{
  Stat* head;
  int size;
  Stat* tail;
} Code;

int yygen( FILE*, FILE* );
Boolean code_generation( Node* );

int yygenerror( Node*, char* );

#endif // __GENERATOR_H__
