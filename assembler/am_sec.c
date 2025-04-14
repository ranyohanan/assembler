/*
 * am_sec.c
 * This file handles the second pass of the assembler.
 * It processes the .am file to handle symbol resolution
 * for entry, external, direct, and relative symbols,
 * and writes output files (.ob, .ent, .ext) accordingly.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "am_sec.h"

static char amFileName[WORD_MAX_SIZE];	/* stores the .am file name */
static char entFileName[WORD_MAX_SIZE]; /* stores the .ent file name */
static char extFileName[WORD_MAX_SIZE]; /* stores the .ext file name */
static char obFileName[WORD_MAX_SIZE];	/* stores the .ob file name */

void secPassHandle(char *fileName)
{
	char line[MAX_LINE];		/* buffer to hold current line from file */
	char *p;					/* pointer to the line */
	char *wth;					/* represent one word in the line to handle - Word To Handle */
	char symbol[WORD_MAX_SIZE]; /* holding the symbol */
	int num_of_line = 0;		/* the current line number */
	int errors = 0;				/* total error count */
	int entries = 0;			/* number of .entry commands */
	int externs = 0;			/* number of externals used */
	FILE *amf;					/* pointer for am file */
	FILE *entf;					/* pointer for ent file */
	FILE *extf;					/* pointer for ext file */
	FILE *obf;					/* pointer for ob file */

	/* construct file names */
	strcpy(amFileName, fileName);
	strcat(amFileName, ".am");
	strcpy(entFileName, fileName);
	strcat(entFileName, ".ent");
	strcpy(extFileName, fileName);
	strcat(extFileName, ".ext");
	strcpy(obFileName, fileName);
	strcat(obFileName, ".ob");

	/* open all required files */
	amf = fopen(amFileName, "r");
	if (!amf)
	{
		printf("\n%s File has failed to open\n", amFileName);
		return;
	}
	entf = fopen(entFileName, "w");
	if (!entf)
	{
		printf("\n%s File has failed to open\n", entFileName);
		return;
	}
	extf = fopen(extFileName, "w");
	if (!extf)
	{
		printf("\n%s File has failed to open\n", extFileName);
		return;
	}
	obf = fopen(obFileName, "w");
	if (!obf)
	{
		printf("\n%s File has failed to open\n", obFileName);
		return;
	}

	while (fgets(line, sizeof(line), amf))
	{
		line[strcspn(line, "\n")] = '\0'; /* remove newline character */
		num_of_line++;
		if (line[0] == ';') /* comment line */
		{
			continue;
		}
		p = line;
		removeLeadingSpaces(p, line);
		wth = strtok(line, " :");
		/* skip those guide words (no symbols/symbols definition only) */
		if (!strcmp(wth, ".data") || !strcmp(wth, ".string") || !strcmp(wth, ".extern"))
		{
			continue;
		}
		/* handle entry command */
		if (!strcmp(wth, ".entry"))
		{
			if (handleEntrySymbol(line, num_of_line, amFileName) == FALSE)
			{
				errors++;
				continue;
			}
			entries++;
			continue;
		}
		/* scan for symbol references in the line */
		wth = strtok(NULL, " ,");
		while (wth != NULL)
		{
			if (wth[0] == '&') /* relative address operand */
			{
				wth++;
				if (checkExistSymbol(wth) == TRUE)
				{
					strcpy(symbol, wth);
					if (relativeSymbolFound(symbol, errors, amFileName, num_of_line) == FALSE)
					{
						errors++;
					}
				}
				else
				{
					printf("\nIn file %s Error in line %d: None existed symbol inside an opcode\n", amFileName, num_of_line);
					errors++;
					break;
				}
				wth = strtok(NULL, " ,");
				continue;
			}
			if (potSymbol(wth) == TRUE) /* normal operand (direct symbol)*/
			{
				if (checkExistSymbol(wth) == TRUE)
				{
					strcpy(symbol, wth);
					externs += symbolFound(symbol, errors, extf); /* extern symbol used if return 1 (.ext file will be written) */
				}
				else
				{
					printf("\nIn file %s Error in line %d: None existed symbol inside an opcode\n", amFileName, num_of_line);
					errors++;
					break;
				}
			}
			wth = strtok(NULL, " ,");
			continue;
		}
	}
	/* if there were no errors and code exists */
	if (errors == 0 && IC > MIN_IC)
	{
		runObjectFile(obf); /* write object file */
		if (entries == 0)	/* no entries */
		{
			remove(entFileName);
		}
		if (externs == 0) /* no externs */
		{
			remove(extFileName);
		}
		else
		{
			runEntFile(entf);
		}
	}
	else /* if there is at least one error */
	{
		remove(obFileName);
		remove(entFileName);
		remove(extFileName);
	}
	fclose(amf);
	fclose(entf);
	fclose(extf);
	fclose(obf);
	clearMemory(); /* clean memory */
}

boolean handleEntrySymbol(char *line, int num_of_line, char *amfileName)
{
	char *wth;		  /*represent one word in the line to handle - Word To Handle*/
	symbol curSymbol; /* holds the symbol structure from the symbol table */

	wth = strtok(NULL, " ");
	/* symbol not found in symbol table */
	if (checkExistSymbol(wth) == FALSE)
	{
		printf("\nIn file %s Error in line %d: None existed symbol after \"entry\" command\n", amFileName, num_of_line);
		return FALSE;
	}
	curSymbol = returnSymbol(wth);
	/* cannot be both external and entry */
	if (curSymbol.sec_pass_type == EXTERNAL)
	{
		printf("\nIn file %s Error in line %d: Symbol cannot be defined as both extern and entry\n", amFileName, num_of_line);
		return FALSE;
	}

	setEntrySymbol(wth); /* mark symbol as entry */
	return TRUE;
}

int symbolFound(char *wth, int errors, FILE *extFile)
{
	symbol curSymbol; /* holds the symbol structure from the symbol table */
	curSymbol = returnSymbol(wth);
	if (curSymbol.sec_pass_type == EXTERNAL)
	{
		binaryForExternal(curSymbol, errors, extFile); /* mark memory cell as external and write to .ext file */
		return 1;									   /* when returning 1 increases the number of externs used so the .ext will be printed */
	}
	else
	{
		/* update the memory value for a direct symbol */
		binaryForDirect(curSymbol);
		return 0;
	}
}

boolean relativeSymbolFound(char *wth, int errors, char *amFileName, int num_of_line)
{
	symbol curSymbol; /* holds the symbol structure from the symbol table */
	curSymbol = returnSymbol(wth);
	if (curSymbol.sec_pass_type == EXTERNAL) /* extern symbols cannot be used in relative addressing */
	{
		printf("\nIn file %s Error in line %d: when symbol defined as extern it cannot used as a relative adress operand\n", amFileName, num_of_line);
		return FALSE;
	}
	else
	{
		binaryForRelative(curSymbol.name, curSymbol.value); /* calculate and store relative distance */
	}
	return TRUE;
}

