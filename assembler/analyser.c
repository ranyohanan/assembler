/*
 * analyser.c
 * This file contains functions used to validate and analyze assembly code,
 * including checking syntax rules, validating operands, identifying opcodes,
 * registers, macros, symbols, and guide words.
 * It supports the macros deployment and the first and second pass of the assembler by assisting in parsing and
 * validation of .am lines.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "mcros.h"
#include "analyser.h"

static char registers[NUM_REGISTERS][THREE] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"}; /* Array of all registers names */
static opcode opcodes[NUM_OPCODES] = {
	{"mov", 0, 0, {IMMEDIATE, DIRECT, REGISTER}, {DIRECT, REGISTER, NONE}},
	{"cmp", 0, 1, {IMMEDIATE, DIRECT, REGISTER}, {IMMEDIATE, DIRECT, REGISTER}},
	{"add", 1, TWO, {IMMEDIATE, DIRECT, REGISTER}, {DIRECT, REGISTER, NONE}},
	{"sub", TWO, TWO, {IMMEDIATE, DIRECT, REGISTER}, {DIRECT, REGISTER, NONE}},
	{"lea", 0, FOUR, {DIRECT, NONE, NONE}, {DIRECT, REGISTER}},
	{"clr", 1, FIVE, {NONE, NONE, NONE}, {DIRECT, REGISTER, NONE}},
	{"not", TWO, FIVE, {NONE, NONE, NONE}, {DIRECT, REGISTER, NONE}},
	{"inc", THREE, FIVE, {NONE, NONE, NONE}, {DIRECT, REGISTER, NONE}},
	{"dec", FOUR, FIVE, {NONE, NONE, NONE}, {DIRECT, REGISTER, NONE}},
	{"jmp", 1, NINE, {NONE, NONE, NONE}, {DIRECT, RELATIVE, NONE}},
	{"bne", TWO, NINE, {NONE, NONE, NONE}, {DIRECT, RELATIVE, NONE}},
	{"jsr", THREE, NINE, {NONE, NONE, NONE}, {DIRECT, RELATIVE, NONE}},
	{"red", 0, TWELVE, {NONE, NONE, NONE}, {DIRECT, REGISTER, NONE}},
	{"prn", 0, THIRTEEN, {NONE, NONE, NONE}, {IMMEDIATE, DIRECT, REGISTER}},
	{"rts", 0, FOURTEEN, {NONE, NONE, NONE}, {NONE, NONE, NONE}},
	{"stop", 0, FIFTEEN, {NONE, NONE, NONE}, {NONE, NONE, NONE}}}; /* Array of all opcodes names */

int quotation(char *p)
{
	int counter = 0; /* counter of quotation marks */
	while (*p != '\n' && *p != EOF)
	{
		if (*p == '"')
		{
			counter++; /* Count each apperance of quotation mark */
		}
		p++;
	}
	return counter;
}

boolean lastIsComma(char *line){
	size_t size = strlen(line);
	if(line[size-1] == ','){
		return TRUE;
	}
	return FALSE;
}

int wordCounter(char *p)
{
	int num_of_words = 0; /* counter for the number of words in the line */
	int wordFlag = OFF;	  /* flag for indicate if the current node is part of a word */
	if (p == NULL)
	{
		return 0;
	}
	while (*p != '\0' && *p != EOF)
	{
		if (isspace(*p) || *p == ',')
		{
			wordFlag = OFF; /* when spoted a non word node*/
		}
		else
		{
			if (wordFlag == OFF)
			{
				num_of_words++;
				wordFlag = ON; /* when spoted a new word node*/
			}
		}
		p++;
	}
	return num_of_words;
}

boolean checkNewMcro(char *mcroName, int nameSize)
{
	int i; /* index for loops */
	/* check if the mcro name length is legal */
	if (nameSize > WORD_MAX_SIZE)
	{
		return FALSE;
	}
	/* check if the mcro name start is legal */
	if (!isalpha(mcroName[0]) && mcroName[0] != '_')
	{
		return FALSE;
	}
	for (i = 0; i < strlen(mcroName); i++)
	{
		if (mcroName[i] == ':')
		{
			return FALSE;
		}
	}
	/* check if the mcro name isn't a register name */
	for (i = 0; i < NUM_REGISTERS; i++)
	{
		if (!strcmp(mcroName, registers[i]))
		{
			return FALSE;
		}
	}
	/* check if the mcro name isn't an opcode name */
	for (i = 0; i < NUM_OPCODES; i++)
	{
		if (!strcmp(mcroName, opcodes[i].name))
		{
			return FALSE;
		}
	}
	/* check if the mcro name isn't a saved name */
	if ((!strcmp(mcroName, "mcro")) || (!strcmp(mcroName, "mcroend")))
	{
		return FALSE;
	}
	if ((!strcmp(mcroName, "entry")) || (!strcmp(mcroName, "data")) || (!strcmp(mcroName, "extern")) || (!strcmp(mcroName, "string")))
	{
		return FALSE;
	}
	return TRUE;
}

boolean doubleCommas(char *line)
{
	int i;						/* index for loops */
	size_t size = strlen(line); /* length of the line */
	int commaFlag = OFF;		/* indicates if the previous character was a comma */
	for (i = 0; i < size; i++)
	{
		if (line[i] == ',')
		{
			if (commaFlag == OFF)
			{
				commaFlag = ON; /* first comma found */
			}
			else
			{
				return TRUE; /* second comma found without an operand in between */
			}
		}
		/* reset flag if a non-space, non-comma character is found */
		if (!isspace(line[i]) && line[i] != ',')
		{
			commaFlag = OFF;
		}
	}
	return FALSE; /* no consecutive commas found */
}

boolean findColon(char *line)
{
	int i;						/* index for loop */
	size_t size = strlen(line); /* length of the line */
	for (i = 0; i < size; i++)
	{
		if (line[i] == ':')
		{
			return TRUE; /* colon found */
		}
	}
	return FALSE; /* no colon found */
}

boolean checkGuide(char *guide)
{
	if (strcmp(guide, ".data") != 0 && strcmp(guide, ".string") != 0 && strcmp(guide, ".entry") != 0 && strcmp(guide, ".extern") != 0)
	{
		return FALSE; /* not a valid guide word */
	}
	return TRUE; /* valid guide word */
}

boolean checkSymbol(char *symbol, int num_of_line, char *amFileName)
{
	int i;							/* index for loop */
	size_t length = strlen(symbol); /* length of the symbol name */
	/* Check length limit */
	if (length > WORD_MAX_SIZE)
	{
		printf("\nIn file %s Error in line %d: length of symbol's name is illegal\n", amFileName, num_of_line);
		return FALSE;
	}
	/* Symbol must start with a letter */
	if (!isalpha(symbol[0]))
	{
		return FALSE;
	}
	/* Symbol must not be a guide word */
	if (!strcmp(symbol, "data") || !strcmp(symbol, "string") || !strcmp(symbol, "extern") || !strcmp(symbol, "entry"))
	{
		printf("\nIn file %s Error in line %d: cant use a guide word as a symbol name\n", amFileName, num_of_line);
		return FALSE;
	}
	/* All characters must be a letter or number */
	for (i = 0; i < length; i++)
	{
		if (!isalnum(symbol[i]))
		{
			return FALSE;
		}
	}
	/* Symbol must not be a macro name */
	if (checkExistMcro(symbol) == TRUE)
	{
		printf("\nIn file %s Error in line %d: cant use a macro name as a symbol name\n", amFileName, num_of_line);
		return FALSE;
	}
	/* Symbol must not be a register */
	if (isRegister(symbol) == TRUE)
	{
		printf("\nIn file %s Error in line %d: cant use a register as a symbol name\n", amFileName, num_of_line);
		return FALSE;
	}
	/* Symbol must not be an opcode name */
	if (isOpcode(symbol) == TRUE)
	{
		printf("\nIn file %s Error in line %d: cant use an opcode name as a symbol name\n", amFileName, num_of_line);
		return FALSE;
	}
	return TRUE; /* If passed all checks */
}

boolean potSymbol(char *symbol)
{
	int i;							/* index for loop */
	size_t length = strlen(symbol); /* length of the symbol name */
	/* Check length limit */
	if (length > WORD_MAX_SIZE)
	{
		return FALSE;
	}
	/* Symbol must start with a letter */
	if (!isalpha(symbol[0]))
	{
		return FALSE;
	}
	/* Symbol must not be a guide word */
	if (!strcmp(symbol, "data") || !strcmp(symbol, "string") || !strcmp(symbol, "extern") || !strcmp(symbol, "entry"))
	{
		return FALSE;
	}
	/* All characters must be a letter or number */
	for (i = 0; i < length; i++)
	{
		if (!isalnum(symbol[i]))
		{
			return FALSE;
		}
	}
	/* Symbol must not be a macro name */
	if (checkExistMcro(symbol) == TRUE)
	{
		return FALSE;
	}
	/* Symbol must not be a register */
	if (isRegister(symbol) == TRUE)
	{
		return FALSE;
	}
	/* Symbol must not be an opcode name */
	if (isOpcode(symbol) == TRUE)
	{
		return FALSE;
	}
	return TRUE; /* If passed all checks */
}

boolean isRegister(char *word)
{
	int i; /* index for loop */
	for (i = 0; i < NUM_REGISTERS; i++)
	{
		/* compare the word to each register name */
		if (!strcmp(word, registers[i]))
		{
			return TRUE; /* match found */
		}
	}
	return FALSE; /* no match found */
}

boolean isOpcode(char *word)
{
	int i; /* index for loop */
	for (i = 0; i < NUM_OPCODES; i++)
	{
		/* compare the word to each opcode name */
		if (!strcmp(word, opcodes[i].name))
		{
			return TRUE; /* match found */
		}
	}
	return FALSE; /* no match found */
}

boolean checkOperand(char *word)
{
	char first_node; /* holds the first character of the operand */
	if (word == NULL)
	{
		return FALSE; /* operand is empty */
	}
	first_node = word[0];
	if (isOpcode(word) == TRUE)
	{
		return FALSE; /* operand cannot be an opcode name */
	}
	if (first_node == '#')
	{
		/* immediate addressing - check if the following string is a valid number */
		return checkNum(word + 1);
	}
	if (isRegister(word) == TRUE)
	{
		return TRUE; /* register addressing is valid */
	}
	if (first_node == '&')
	{
		word++; /* skip '&' and check the rest as a symbol */
		return potSymbol(word);
	}
	/* default case: check if it's a valid symbol name */
	return potSymbol(word);
}

boolean checkNum(char *word)
{
	int i = 0; /* index for string */
	if (word == NULL || word[0] == '\0')
	{
		return FALSE; /* null or empty string is not valid */
	}
	if (word[0] == '-')
	{
		i++; /* skip the minus sign for further digit checks */
	}
	if (!isdigit(word[i]))
	{
		return FALSE; /* first character or first after '-'(if there is) must be a digit */
	}
	while (word[i] != '\0')
	{
		if (!isdigit(word[i]))
		{
			return FALSE; /* all characters must be digits */
		}
		i++;
	}
	return TRUE; /* valid number */
}

boolean checkString(char *string)
{
	size_t size = strlen(string); /* calculate the length of the string */
	if (string[0] != '"' || string[size - 1] != '"')
	{
		return FALSE; /* string must start and end with a double quote */
	}
	return TRUE; /* valid string */
}

boolean operand1ToOpcode(operand operand1, operand operand2, char *opcode)
{
	int i; /* index for loop */
	int j; /* index for loop */
	/* when there are two operands */
	if (operand2.type != NONE)
	{
		for (i = 0; i < NUM_OPCODES; i++)
		{
			if (!strcmp(opcode, opcodes[i].name)) /* match the opcode name */
			{
				for (j = 0; j < THREE; j++)
				{
					/* check if the operand type is valid as a source */
					if (operand1.type == opcodes[i].source_op[j])
					{
						return TRUE;
					}
				}
				return FALSE; /* no matching addressing type found */
			}
		}
		return FALSE; /* opcode not found */
	}
	/* when only one operand exists, it's considered a destination */
	else
	{
		for (i = 0; i < NUM_OPCODES; i++)
		{
			if (!strcmp(opcode, opcodes[i].name)) /* match the opcode name */
			{
				for (j = 0; j < THREE; j++)
				{
					/* check if the operand type is valid as a destination */
					if (operand1.type == opcodes[i].dest_op[j])
					{
						return TRUE;
					}
				}
				return FALSE; /* no matching addressing type found */
			}
		}
		return FALSE; /* opcode not found */
	}
}

boolean operand2ToOpcode(operand operand, char *opcode)
{
	int i; /* index for loop */
	int j; /* index for loop */
	for (i = 0; i < NUM_OPCODES; i++)
	{
		if (!strcmp(opcode, opcodes[i].name)) /* match the opcode name */
		{
			for (j = 0; j < THREE; j++)
			{
				/* check if the operand type matches one of the valid destination types */
				if (operand.type == opcodes[i].dest_op[j])
				{
					return TRUE;
				}
			}
			return FALSE; /* no matching addressing type found */
		}
	}
	return FALSE; /* opcode not found */
}

int returnOpcodeNum(char *opcode)
{
	int i;	 /* index for loop */
	int num; /* variable to store the opcode number */
	for (i = 0; i < NUM_OPCODES; i++)
	{
		if (!strcmp(opcode, opcodes[i].name)) /* match found */
		{
			num = opcodes[i].code; /* store the opcode number */
		}
	}
	return num; /* returns the matched opcode number */
}

int returnFunct(char *opcode)
{
	int i;		  /* index for loop */
	int functNum; /* variable to store the opcode funct */
	for (i = 0; i < NUM_OPCODES; i++)
	{
		if (!strcmp(opcode, opcodes[i].name)) /* match found */
		{
			functNum = opcodes[i].funct; /* store the opcode funct */
		}
	}
	return functNum; /* returns the matched opcode funct */
}

int returnReg(char *reg)
{
	int regNum;			   /* variable to store the register number */
	regNum = reg[1] - '0'; /* extract the digit after 'r' and convert from char to int */
	return regNum;		   /* return the register number */
}

void decimalToPrint(int value, char *decimal_adress)
{
	int i = DECIMAL_SIZE - TWO;				 /* start filling from the rightmost digit (excluding null terminator) */
	decimal_adress[DECIMAL_SIZE - 1] = '\0'; /* null-terminate the string */
	/* Fill in the digits from right to left */
	while (value > 0)
	{
		decimal_adress[i] = '0' + value % TEN; /* convert digit to char and store */
		value = value / TEN;				   /* remove the last digit */
		i--;
	}
	/* Fill remaining positions with '0' */
	while (i >= 0)
	{
		decimal_adress[i] = '0';
		i--;
	}
}

int wordCounter_for_am(char *p)
{
	int num_of_words = 0; /* counter for the number of words in the line */
	int wordFlag = OFF;	  /* flag for indicate if the current node is part of a word */
	if (p == NULL)
	{
		return 0;
	}
	while (*p != '\0' && *p != EOF)
	{
		if (isspace(*p) || *p == ',' || *p == '"')
		{
			wordFlag = OFF; /* when spoted a non word node*/
		}
		else
		{
			if (wordFlag == OFF)
			{
				num_of_words++;
				wordFlag = ON; /* when spoted a new word node*/
			}
		}
		p++;
	}
	return num_of_words;
}

void removeLeadingSpaces(char *p, char *line)
{
	while (isspace(*p)) /* advance the pointer past any leading whitespace */
	{
		p++;
	}

	while (*p) /* copy the remaining characters into 'line' */
	{
		*line++ = *p++;
	}
	*line = '\0';
}

