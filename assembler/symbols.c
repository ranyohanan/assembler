/*
 * symbols.c
 * This file manages the symbol table for the assembler.
 * It allows adding new symbols, locate them, and generating output for .ent and .ext files.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "symbols.h"
#include "mcros.h"
#include "analyser.h"

static int symbol_count = 0;   /* counter for the number of symbols added */
static symbol *symbols = NULL; /* pointer for the symbols table */

boolean checkExistSymbol(char *symbol)
{
	int i; /* index for loops */
	if (symbols != NULL)
	{
		for (i = 0; i < symbol_count; i++)
		{
			if (!strcmp(symbols[i].name, symbol)) /* symbol name exist */
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void addNewSymbol(char *symbolName, boolean isOpcode, boolean isExtern, boolean isData)
{
	symbol *temp; /* temporary pointer for safer reallocation*/

	/* increasing the storage */
	if (symbols == NULL)
	{
		symbols = malloc(sizeof(symbol));
		if (!symbols)
		{
			printf("\nFailed to allocate memory for symbols\n");
			exit(0);
		}
	}
	else
	{
		temp = realloc(symbols, (symbol_count + 1) * sizeof(symbol));
		if (!temp)
		{
			printf("\nfaild to reallocate memory\n");
			exit(0);
		}
		symbols = temp;
	}
	symbol_count++; /* increasing the count of symbols */
	/* set fields for the new symbol */
	strcpy(symbols[symbol_count - 1].name, symbolName);
	symbols[symbol_count - 1].value = IC;
	symbols[symbol_count - 1].first_pass_type = NO_TYPE1;
	symbols[symbol_count - 1].sec_pass_type = NO_TYPE2;
	if (isOpcode == TRUE)
	{
		symbols[symbol_count - 1].first_pass_type = CODE;
	}
	if (isData == TRUE)
	{
		symbols[symbol_count - 1].first_pass_type = DATA;
	}
	if (isExtern == TRUE)
	{
		symbols[symbol_count - 1].value = 0;
		symbols[symbol_count - 1].sec_pass_type = EXTERNAL;
	}
}

symbol returnSymbol(char *potSymbol)
{
	int i; /* index for loops */
	if (symbols != NULL)
	{
		for (i = 0; i < symbol_count; i++)
		{
			if (!strcmp(symbols[i].name, potSymbol)) /* symbol name found - return the symbol */
			{
				return symbols[i];
			}
		}
	}
	return symbols[i];
}

void runEntFile(FILE *entFile)
{
	int i;						/* index for loops */
	char decimal[DECIMAL_SIZE]; /* string buffer for address in decimal */
	for (i = 0; i < symbol_count; i++)
	{
		if (symbols[i].sec_pass_type == ENTRY)
		{
			decimalToPrint(symbols[i].value, decimal);				/* build decimal adress */
			fprintf(entFile, "%s\t%s\n", symbols[i].name, decimal); /* print symbol and his decimal adress to .ent file */
		}
	}
}

void setEntrySymbol(char *wth)
{
	int i;
	for (i = 0; i < symbol_count; i++)
	{
		if (!strcmp(symbols[i].name, wth))
		{
			symbols[i].sec_pass_type = ENTRY; /* mark the requested symbol as ENTRY */
		}
	}
}

void free_symbols()
{
	if (symbols != NULL)
	{
		free(symbols); /* free the allocate memory of the dynamic symbol table */
		symbols = NULL;
		symbol_count = 0; /* initialize the count back to 0 */
	}
}

