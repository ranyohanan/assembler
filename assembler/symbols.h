#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "analyser.h"

typedef enum
{
    NO_TYPE1, /* No type assigned in first pass */
    DATA,     /* Symbol defined with .data or .string */
    CODE      /* Symbol used in opcode */
} type;       /* Enum for first pass type: NONE, DATA, or CODE */

typedef enum
{
    NO_TYPE2, /* No type assigned in second pass */
    ENTRY,    /* Symbol declared with .entry */
    EXTERNAL  /* Symbol declared with .extern */
} type2;      /* Enum for second pass type: NONE, ENTRY, or EXTERNAL */

typedef struct
{
    int value;                /* value of the symbol */
    char name[WORD_MAX_SIZE]; /* Symbol name */
    type first_pass_type;     /* Classification from first pass (DATA or CODE) */
    type2 sec_pass_type;      /* Classification from second pass (ENTRY or EXTERNAL) */
} symbol;                     /* Structure for a symbol in the symbol table */

/* checks if a symbol already exists in the symbol table */
boolean checkExistSymbol(char *symbol);
/* adds a new symbol to the symbol table */
void addNewSymbol(char *symbolName, boolean isOpcode, boolean isExtern, boolean isData);
/* frees all memory allocated for the symbol table */
void free_symbols();
/* returns a symbol struct by its name */
symbol returnSymbol(char *potSymbol);
/* sets an existing symbol as ENTRY */
void setEntrySymbol(char *wth);
/* writes all entry symbols to the .ent file */
void runEntFile(FILE *entFile);
#endif

