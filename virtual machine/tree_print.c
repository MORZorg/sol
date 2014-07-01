#include "tree_print.h"

const char* CODE_OPERATORS[] =
{
  "NEW",
  "NEWS",
  "LDC",
  "LDI",
  "LDR",
  "LDS",
  "LOD",
  "CAT",
  "LDA",
  "FDA",
  "EIL",
  "SIL",
  "IXA",
  "STO",
  "IST",
  "JMF",
  "JMP",
  "EQU",
  "NEQ",
  "CGT",
  "CGE",
  "CLT",
  "CLE",
  "IGT",
  "IGE",
  "ILT",
  "ILE",
  "RGT",
  "RGE",
  "RLT",
  "RLE",
  "SGT",
  "SGE",
  "SLT",
  "SLE",
  "IN",
  "IPLUS",
  "IMINUS",
  "ITIMES",
  "IDIV",
  "RPLUS",
  "RMINUS",
  "RTIMES",
  "RDIV",
  "IUMI",
  "RUMI",
  "NEG",
  "WR",
  "FWR",
  "PUSH",
  "GOTO",
  "POP",
  "RD",
  "FRD",
  "TOINT",
  "TOREAL",
  "READ",
  "FREAD",
  "WRITE",
  "FWRITE",
  "FUNC",
  "RETURN",
  "SCODE",
  "HALT",
};

void code_array_print( Stat* code, int program_size )
{
	int i;
	for( i = 0; i < program_size; i++ )
	{
		switch( code[ i ].op )
		{
			case SOL_FREAD:
			case SOL_READ:
				fprintf( stdout, "%s %d %d \"%s\"\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].i_val,
						code[ i ].args[ 1 ].i_val,
						code[ i ].args[ 2 ].s_val );
				break;

			case SOL_LDC:
				fprintf( stdout, "%s '%c'\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].c_val );
				break;

			case SOL_FRD:
			case SOL_FWR:
			case SOL_FWRITE:
			case SOL_LDS:
			case SOL_RD:
			case SOL_WR:
			case SOL_WRITE:
				fprintf( stdout, "%s \"%s\"\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].s_val );
				break;

			case SOL_LDR:
				fprintf( stdout, "%s %f\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].r_val );
				break;

			case SOL_CAT:
			case SOL_LDA:
			case SOL_LOD:
			case SOL_PUSH:
			case SOL_STO:
				fprintf( stdout, "%s %d %d\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].i_val,
						code[ i ].args[ 1 ].i_val );
				break;

			case SOL_EQU:
			case SOL_IGE:
			case SOL_IGT:
			case SOL_IPLUS:
			case SOL_IST:
			case SOL_ITIMES:
			case SOL_IUMI:
			case SOL_NEG:
			case SOL_NEQ:
			case SOL_POP:
			case SOL_RETURN:
			case SOL_RPLUS:
			case SOL_RTIMES:
			case SOL_TOINT:
			case SOL_TOREAL:
			case SOL_HALT:
				fprintf( stdout, "%s\n",
						CODE_OPERATORS[ code[ i ].op ] );
				break;

			default:
				fprintf( stdout, "%s %d\n",
						CODE_OPERATORS[ code[ i ].op ],
						code[ i ].args[ 0 ].i_val );
				break;
		}
	}
}

const char* get_operator_name( Operator op )
{
	return CODE_OPERATORS[ op ];
}
