/*
 * am_first.c
 * This file performs the first pass on the .am file.
 * It pass each line to identify labels, guide words (.data, .string, .entry, .extern), and instructions.
 * It builds the symbol table and create part of the memory accordingly.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "am_first.h"

static char amFileName[WORD_MAX_SIZE]; /* holds the .am file name */
static char newSymbol[WORD_MAX_SIZE];  /* holds the symbol name to define */
static int errors;					   /* counter for errors encountered */

int firstPassHandle(char *fileName)
{
	char line[MAX_LINE];			  /* holds current line from file */
	char *p;						  /* pointer to the current line */
	char *wth;						  /* represent one word in the line to handle - Word To Handle */
	char guide_word[GUIDE_WORD_SIZE]; /* holds a guide word */
	char opcode[FOUR];				  /* holds the name of an opcode */
	int num_of_line = 0;			  /* current line number in the file */
	int num_of_words = 0;			  /* number of words in the current line */
	int symbol_line;				  /* flag indicating if the line defines a symbol */
	int guide_line;					  /* flag indicating if the line is a guide word line */
	FILE *amf;						  /*pointer for .am file*/
	IC = MIN_IC;					  /* initialize instruction counter */
	DC = 0;							  /* initialize data counter */
	errors = 0;						  /* initialize error count */

	/* build the .am file name */
	strcpy(amFileName, fileName);
	strcat(amFileName, ".am");

	amf = fopen(amFileName, "r");
	if (!amf)
	{
		printf("\n%s File has failed to open\n", amFileName);
		return 1;
	}

	/* read file line by line */
	while (fgets(line, sizeof(line), amf))
	{
		line[strcspn(line, "\n")] = '\0'; /* remove enter node */
		num_of_line++;
		symbol_line = OFF;
		guide_line = OFF;

		/* skip comment lines */
		if (line[0] == ';')
		{
			continue;
		}

		p = line;

		/* counting the number of words in the line */
		num_of_words = wordCounter_for_am(p);

		removeLeadingSpaces(p, line);
		if(lastIsComma(line) == TRUE){
			printf("\nIn file %s Error in line %d: Illegal use of comma in assembly\n", amFileName, num_of_line);
			errors++;
			continue;
		}
		/* check for double commas */
		if (doubleCommas(line) == TRUE)
		{
			printf("\nIn file %s Error in line %d: Detected two or more consecutive commas\n", amFileName, num_of_line);
			errors++;
			continue;
		}

		/* check if line contains a symbol definition (has a colon) */
		if (findColon(line) == TRUE)
		{
			wth = strtok(line, ":");
			if (wth != NULL)
			{
				if (checkSymbol(wth, num_of_line, amFileName) == TRUE && checkExistSymbol(wth) == FALSE)
				{
					symbol_line = ON;
					strcpy(newSymbol, wth);
					wth = strtok(NULL, " \t\n");
				}
				else
				{
					printf("\nIn file %s Error in line %d: Illegal symbol name or trying to define an already exist symbol\n", amFileName, num_of_line);
					errors++;
					continue;
				}
			}
		}
		else
		{
			wth = strtok(line, " \t\n"); /* get the first word */
		}

		/* check for guide word (.data, .string, ...) */
		if (wth[0] == '.')
		{
			if (checkGuide(wth) == TRUE)
			{
				guide_line = ON;
				strcpy(guide_word, wth);
				if (symbol_line == ON)
				{
					handleSymbolGuide(line, newSymbol, guide_word, amFileName, num_of_line, num_of_words);
				}
				else
				{
					handleGuide(line, guide_word, amFileName, num_of_line, num_of_words);
				}
				continue;
			}
			else
			{
				printf("\nIn file %s Error in line %d: Using of none existed guide word\n", amFileName, num_of_line);
				errors++;
				continue;
			}
		}

		/* handle opcode (if not a guide word) */
		if (guide_line == OFF)
		{
			if (isOpcode(wth) == TRUE)
			{
				strcpy(opcode, wth);
				if (symbol_line == ON)
				{
					handleSymbolOpcode(line, newSymbol, opcode, amFileName, num_of_line, num_of_words);
				}
				else
				{
					handleOpcode(line, opcode, amFileName, num_of_line, num_of_words);
				}
				continue;
			}
			else
			{
				printf("\nIn file %s Error in line %d: Using of none existed opcode\n", amFileName, num_of_line);
				errors++;
				continue;
			}
		}
		else
		{
			printf("\nIn file %s Error in line %d: Unrecognized command\n", amFileName, num_of_line);
			errors++;
		}
	}
	fclose(amf);   /* close file after reading */
	if(errors != 0){
		clearMemory();
	}
	return errors; /* return number of errors */
}

void handleSymbolGuide(char *line, char *symbol, char *guide_word, char *amFileName, int num_of_line, int num_of_words)
{
	char *p;				  /*pointer to the line*/
	char *wth;				  /* represent one word in the line to handle - Word To Handle */
	char *string;			  /* holds string from .string guide word */
	int integers[MAX_LINE];	  /* array to store integers from .data */
	size_t string_size;		  /* the size of the string from .string guide word*/
	int num_of_int = 0;		  /* counter for number of valid integers collected */
	int num_of_nodes = 0;	  /* number of characters in .string (excluding quotes) */
	boolean isExtern = FALSE; /* a boolean that sends to addNewSymbol and indicate if the symbol type is external */
	boolean isOpcode = FALSE; /* a boolean that sends to addNewSymbol and indicate if the symbol type is code */
	boolean isData;			  /* a boolean that sends to addNewSymbol and indicate if the symbol type is data */
	p = line;
	removeLeadingSpaces(p, line);

	/* check if guide word is .entry which is illegal after symbol */
	if (!strcmp(guide_word, ".entry"))
	{
		printf("\nIn file %s Error in line %d: Cannot define a symbol before \".entry\" command\n", amFileName, num_of_line);
		errors++;
		return;
	}
	/* check if guide word is .extern which is also illegal after symbol */
	if (!strcmp(guide_word, ".extern"))
	{
		printf("\nIn file %s Error in line %d: Cannot define a symbol before \".extern\" command\n", amFileName, num_of_line);
		errors++;
		return;
	}
	/* handle .data guide word */
	if (!strcmp(guide_word, ".data"))
	{
		wth = strtok(NULL, " ,");
		while (wth != NULL)
		{
			while (isspace(wth[0]))
			{
				wth++;
			}
			if (checkNum(wth) == TRUE)
			{
				num_of_int++;
				integers[num_of_int - 1] = atoi(wth); /* convert string to int and store */
			}
			else
			{
				printf("\nIn file %s Error in line %d: \".data\" command values are not a valid integers\n", amFileName, num_of_line);
				errors++;
				return;
			}
			wth = strtok(NULL, ","); /* get next number */
		}

		/* validate number of integers is legal */
		if (num_of_int != num_of_words - TWO)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".data\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		if (num_of_int == 0)
		{
			printf("\nIn file %s Error in line %d: No integers sent to \".data\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		isData = TRUE;
		addNewSymbol(symbol, isOpcode, isExtern, isData); /* add symbol to symbol table */
		buildCodeData(integers, num_of_int);			  /* add integers to memory */
		return;
	}

	/* handle .string guide word */
	if (!strcmp(guide_word, ".string"))
	{
		string = strtok(NULL, "\0"); /* get string including quotes */
		if (string == NULL)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".string\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		string_size = strlen(string);
		if (string == NULL || checkString(string) == FALSE)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".string\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		num_of_nodes = string_size - 1; /* all nodes of the string minus the two quotation marks plus a '\0' node */
		isData = TRUE;
		addNewSymbol(symbol, isOpcode, isExtern, isData); /* add symbol to symbol table */
		buildCodeString(string, num_of_nodes);			  /* add string to memory */
		return;
	}
}

void handleGuide(char *line, char *guide_word, char *amFileName, int num_of_line, int num_of_words)
{
	char *p;				  /* pointer to the line */
	char *wth;				  /* represent one word in the line to handle - Word To Handle */
	char *string;			  /* holds string from .string guide word */
	size_t string_size;		  /* the size of the string from .string guide word*/
	int integers[MAX_LINE];	  /* array to store integers from .data */
	int num_of_int = 0;		  /* counter for number of valid integers collected */
	int num_of_nodes = 0;	  /* number of characters in .string (excluding quotes) */
	boolean isExtern;		  /* a boolean that sends to addNewSymbol and indicate if the symbol type is external */
	boolean isOpcode = FALSE; /* a boolean that sends to addNewSymbol and indicate if the symbol type is code */
	boolean isData = FALSE;	  /* a boolean that sends to addNewSymbol and indicate if the symbol type is data */
	p = line;
	removeLeadingSpaces(p, line);
	/* handle .entry guide word */
	if (!strcmp(guide_word, ".entry"))
	{
		wth = strtok(NULL, " ");
		if (wth == NULL || checkSymbol(wth, num_of_line, amFileName) == FALSE || num_of_words != TWO)
		{
			printf("\nIn file %s Error in line %d: No symbol detected after entry command or Illegal setting of \".entry\" command\n", amFileName, num_of_line);
			errors++;
		}
		return;
	}
	/* handle .extern guide word */
	if (!strcmp(guide_word, ".extern"))
	{
		wth = strtok(NULL, " ");
		if (wth == NULL)
		{
			printf("\nIn file %s Error in line %d: No symbol detected after \".extern\" command\n", amFileName, num_of_line);
			errors++;
			return;
		}
		if (potSymbol(wth) == TRUE && checkExistSymbol(wth) == FALSE && num_of_words == TWO)
		{
			isExtern = TRUE;
			addNewSymbol(wth, isOpcode, isExtern, isData); /* add the extern symbol */
		}
		else
		{
			/* possible errors for .extern line */
			if (checkExistSymbol(wth) == TRUE)
			{
				printf("\nIn file %s Error in line %d: Using an already exist symbol as an extern\n", amFileName, num_of_line);
				errors++;
			}
			if (checkSymbol(wth, num_of_line, amFileName) == FALSE)
			{
				printf("\nIn file %s Error in line %d: Illegal symbol name on extern line\n", amFileName, num_of_line);
				errors++;
			}
			if (num_of_words != TWO)
			{
				printf("\nIn file %s Error in line %d: Illegal setting of \".extern\" command\n", amFileName, num_of_line);
				errors++;
			}
		}
		return;
	}
	/* handle .data guide word */
	if (!strcmp(guide_word, ".data"))
	{
		wth = strtok(NULL, " ,");
		while (wth != NULL)
		{
			while (isspace(wth[0]))
			{
				wth++;
			}
			if (checkNum(wth) == TRUE)
			{
				num_of_int++;
				integers[num_of_int - 1] = atoi(wth); /* convert string to int and store */
			}
			else
			{
				printf("\nIn file %s Error in line %d: \".data\" command values are not a valid integers\n", amFileName, num_of_line);
				errors++;
				return;
			}
			wth = strtok(NULL, ","); /* get next number */
		}
		/* validate number of integers is legal */
		if (num_of_int != num_of_words - 1)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".data\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		if (num_of_int == 0)
		{
			printf("\nIn file %s Error in line %d: No integers sent to \".data\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		buildCodeData(integers, num_of_int);
		return;
	}
	/* handle .string guide word */
	if (!strcmp(guide_word, ".string"))
	{
		string = strtok(NULL, "\0"); /* get the string */
		if (string == NULL)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".string\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		string_size = strlen(string);
		if (string == NULL || checkString(string) == FALSE)
		{
			printf("\nIn file %s Error in line %d: Illegal setting of \".string\" command line\n", amFileName, num_of_line);
			errors++;
			return;
		}
		num_of_nodes = string_size - 1;		   /* all nodes of the string minus the two quotation marks plus a '\0' node */
		buildCodeString(string, num_of_nodes); /* add integers to memory */
		return;
	}
}

void handleSymbolOpcode(char *line, char *symbol, char *opcode, char *amFileName, int num_of_line, int num_of_words)
{
	char *p;				  /* pointer to the line */
	char *wth;				  /* represent one word in the line to handle - Word To Handle */
	operand operand1;		  /* first operand in the instruction */
	operand operand2;		  /* second operand in the instruction */
	boolean isExtern = FALSE; /* a boolean that sends to addNewSymbol and indicate if the symbol type is external */
	boolean isOpcode = TRUE;  /* a boolean that sends to addNewSymbol and indicate if the symbol type is code */
	boolean isData = FALSE;	  /* a boolean that sends to addNewSymbol and indicate if the symbol type is data */
	p = line;
	removeLeadingSpaces(p, line);
	wth = strtok(NULL, " ,"); /* get first operand or NULL */
	/* check for too many operands */
	if (num_of_words > FOUR)
	{
		printf("\nIn file %s Error in line %d: Illegal number of operands in the line\n", amFileName, num_of_line);
		errors++;
		return;
	}
	/* handle no-operand opcodes */
	if (wth == NULL)
	{
		if (!strcmp(opcode, "rts") || !strcmp(opcode, "stop"))
		{
			operand1.type = NONE;
			operand2.type = NONE;
			addNewSymbol(symbol, isOpcode, isExtern, isData);
			buildCodeOpcode(opcode, operand1, operand2);
			return;
		}
		else
		{
			printf("\nIn file %s Error in line %d: No operands were set in the opcode\n", amFileName, num_of_line);
			errors++;
			return;
		}
	}
	/* handle first operand */
	operand1 = setOperand(wth, symbol, amFileName, num_of_line);
	if (operand1.type == NONE)
	{
		printf("\nIn file %s Error in line %d: First operand of opcode line is illegal\n", amFileName, num_of_line);
		errors++;
		return;
	}
	wth = strtok(NULL, " ,"); /* get second operand (if exists) */
	/* handle second operand if exists */
	if (wth != NULL)
	{
		operand2 = setOperand(wth, symbol, amFileName, num_of_line);
		if (operand2.type == NONE)
		{
			printf("\nIn file %s Error in line %d: Second operand of opcode line is illegal\n", amFileName, num_of_line);
			errors++;
			return;
		}
	}
	else
	{
		operand2.type = NONE;
	}
	/* check if addressing methods are valid for this opcode */
	if (operand1ToOpcode(operand1, operand2, opcode) == FALSE)
	{
		printf("\nIn file %s Error in line %d: first operand adressing isn't suitable the \"%s\" opcode\n", amFileName, num_of_line, opcode);
		errors++;
		return;
	}
	if (operand2.type != NONE)
	{
		if (operand2ToOpcode(operand2, opcode) == FALSE)
		{
			printf("\nIn file %s Error in line %d: second operand adressing isn't suitable the \"%s\" opcode\n", amFileName, num_of_line, opcode);
			errors++;
			return;
		}
	}
	addNewSymbol(symbol, isOpcode, isExtern, isData); /* add the symbol to the symbol table */
	buildCodeOpcode(opcode, operand1, operand2);	  /* create an opcode memory */
}

void handleOpcode(char *line, char *opcode, char *amFileName, int num_of_line, int num_of_words)
{
	char *p;			/*pointer to the line*/
	char symbol[] = ""; /* for send it to setOperand when there is no defining of symbol in the line */
	char *wth;			/* represent one word in the line to handle - Word To Handle */
	operand operand1;	/* first operand in the instruction */
	operand operand2;	/* second operand in the instruction */

	p = line;
	removeLeadingSpaces(p, line);
	wth = strtok(NULL, " ,"); /* get first operand or NULL */
	/* check for too many operands */
	if (num_of_words > THREE)
	{
		printf("\nIn file %s Error in line %d: Illegal number of operands in the line\n", amFileName, num_of_line);
		errors++;
		return;
	}
	/* handle no-operand opcodes */
	if (wth == NULL)
	{
		if (!strcmp(opcode, "rts") || !strcmp(opcode, "stop"))
		{
			operand1.type = NONE;
			operand2.type = NONE;
			buildCodeOpcode(opcode, operand1, operand2);
			return;
		}
		else
		{
			printf("\nIn file %s Error in line %d: No operands were set in the opcode\n", amFileName, num_of_line);
			errors++;
			return;
		}
	}
	/* handle first operand */
	operand1 = setOperand(wth, symbol, amFileName, num_of_line);
	if (operand1.type == NONE)
	{
		printf("\nIn file %s Error in line %d: First operand of opcode line is illegal\n", amFileName, num_of_line);
		errors++;
		return;
	}
	wth = strtok(NULL, " ,"); /* get second operand (if exists) */
	/* handle second operand if exists */
	if (wth != NULL)
	{
		operand2 = setOperand(wth, symbol, amFileName, num_of_line);
		if (operand2.type == NONE)
		{
			printf("\nIn file %s Error in line %d: Second operand of opcode line is illegal\n", amFileName, num_of_line);
			errors++;
			return;
		}
	}
	else
	{
		operand2.type = NONE;
	}
	/* check if addressing methods are valid for this opcode */
	if (operand1ToOpcode(operand1, operand2, opcode) == FALSE)
	{
		printf("\nIn file %s Error in line %d: Operand adressing isn't suitable the \"%s\" opcode\n", amFileName, num_of_line, opcode);
		errors++;
		return;
	}
	if (operand2.type != NONE)
	{
		if (operand2ToOpcode(operand2, opcode) == FALSE)
		{
			printf("\nIn file %s Error in line %d: second operand adressing isn't suitable the \"%s\" opcode\n", amFileName, num_of_line, opcode);
			errors++;
			return;
		}
	}
	buildCodeOpcode(opcode, operand1, operand2); /* create an opcode memory */
}

operand setOperand(char *wth, char *symbol, char *amFileName, int num_of_line)
{
	operand ope; /* operand struct to return */
	/* validate operand and ensure it's not a macro name */
	if (checkOperand(wth) == FALSE || checkExistMcro(wth) == TRUE)
	{
		ope.type = NONE;
		return ope;
	}
	/* when register addressing: operand is a register name */
	if (isRegister(wth) == TRUE)
	{
		ope.type = REGISTER;
		strcpy(ope.content, wth);
		return ope;
	}
	/* when direct addressing: operand is a valid symbol name, and not the symbol being defined */
	if (potSymbol(wth) == TRUE && strcmp(wth, symbol) != 0)
	{
		ope.type = DIRECT;
		strcpy(ope.content, wth);
		return ope;
	}
	/* when immediate addressing: starts with '#' followed by a number */
	if (wth[0] == '#')
	{
		wth++;
		if (checkNum(wth) == FALSE)
		{
			ope.type = NONE;
			return ope;
		}
		ope.type = IMMEDIATE;
		strcpy(ope.content, wth);
		return ope;
	}
	/* when relative addressing: starts with '&' followed by a symbol */
	if (wth[0] == '&')
	{
		wth++;
		if (potSymbol(wth) == TRUE && strcmp(wth, symbol) != 0)
		{
			ope.type = RELATIVE;
			strcpy(ope.content, wth);
			return ope;
		}
		else
		{
			ope.type = NONE;
			return ope;
		}
	}
	/* if none of the above matched, mark as invalid */
	ope.type = NONE;
	return ope;
}


