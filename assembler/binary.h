#ifndef BINARY_H
#define BINARY_H

#include "analyser.h"
#include "symbols.h"

#define NUM_OF_BITS 24 /* total number of bits in one memory unit */
#define HEXA_SIZE 7	   /* hex string length (6 digits + null terminator) */
#define EIGHT 8
#define ELEVEN 11
#define SIXTEEN 16
#define EIGHTEEN 18
#define TWENTY_ONE 21
#define MEMORY_SIZE (1 << 21)	  /* maximum number of memory units (2^21) */
#define BITS_MASK ((1 << 24) - 1) /* mask to keep only the lowest 24 bits */

/* binary code of opcode */
typedef struct
{
	unsigned int E : 1;				  /* External flag */
	unsigned int R : 1;				  /* Relocatable flag */
	unsigned int A : 1;				  /* Absolute flag */
	unsigned int funct : FIVE;		  /* Opcode funct code */
	unsigned int dest_reg : THREE;	  /* Destination register number */
	unsigned int dest_adress : TWO;	  /* Destination operand addressing type */
	unsigned int source_reg : THREE;  /* Source register number */
	unsigned int source_adress : TWO; /* Source operand addressing type */
	unsigned int opcode_num : SIX;	  /* Opcode number */
} opcode_binary;

/* binary code of the value of a label or a number */
typedef struct
{
	unsigned int E : 1;		/* External flag */
	unsigned int R : 1;		/* Relocatable flag */
	unsigned int A : 1;		/* Absolute flag */
	int value : TWENTY_ONE; /* Operand value (21 bits) */
} value_binary;

/* binary code of one node in a guide line (ascii/integer)*/
typedef struct
{
	int value : NUM_OF_BITS; /* Full 24-bit value */
} full_binary;

/* struct to include the binary code of each line + his number in the memory */
typedef struct
{
	char content[WORD_MAX_SIZE]; /* only for symbol operand memory - if not a symnol operand contain "" */
	int num;					 /* Address in memory */
	adress_type adress;			 /* Type of address (opcode, direct, ...) */
	union
	{
		opcode_binary main_code; /* Used if this is an opcode line */
		value_binary value_code; /* Used for operands */
		full_binary full_code;	 /* Used for .data/.string */
	} code;
} memory_unit;

/* Builds code for opcode line */
void buildCodeOpcode(char *opcode, operand operand1, operand operand2);
/* Builds code for .data guide word */
void buildCodeData(int *integers, int num_of_int);
/* Builds code for .string guide word */
void buildCodeString(char *string, int num_of_nodes);
/* Adds ASCII character code to memory */
void addAsciiCode(int node);
/* Adds integer data to memory */
void addDataCode(int integer);
/* Adds opcode instruction to memory */
void addBinaryOpcode(operand operand1, operand operand2, int opcode, int opcode_funct);
/* Adds operand binary code to memory */
void addBinaryOperand(operand ope);
/* Inserts memory unit into memory image */
void addNewMemory(memory_unit new_unit);
/* Clears all memory entries */
void clearMemory();
/* Converts memory unit to binary + hex and prints */
void memory_translate(memory_unit unit, FILE *obFile);
/* Prints 24-bit binary value in hex */
void print_hexa(int binary_value, FILE *obFile, int num);
/* Updates value of direct symbols in the second pass */
void binaryForDirect(symbol curSymbol);
/* Updates value of relative symbols in the second pass */
void binaryForRelative(char *symbolName, int symbolValue);
/* Marks symbol as external and writes to .ext file */
void binaryForExternal(symbol curSymbol, int errors, FILE *extFile);
/* Updates memory for external symbol and returns its address */
int changeMemoryExternal(char *curSymbol);
/* Writes the .ob file with all memory image data */
void runObjectFile(FILE *obFile);
#endif

