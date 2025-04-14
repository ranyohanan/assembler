/*
 * binary.c
 * This file handles the creation and management of the binary memory image during assembly.
 * It translates opcodes, operands, data, strings, and symbolic references into binary code,
 * and outputs this data in hexadecimal formats.
 */ 
#include<string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "analyser.h"
#include "binary.h"

static int memory_count = 0;			/* number of memory units used */
static memory_unit memory[MEMORY_SIZE]; /* array to store all the memory */

void buildCodeOpcode(char *opcode, operand operand1, operand operand2)
{
	int opcode_num;	  /* holds numeric value of the opcode */
	int opcode_funct; /* holds the funct number of the given opcode */
	/* Get opcode numeric value and funct number */
	opcode_num = returnOpcodeNum(opcode);
	opcode_funct = returnFunct(opcode);
	/* Add the main instruction word to memory */
	addBinaryOpcode(operand1, operand2, opcode_num, opcode_funct);
	IC++; /* increment instruction counter */
	/* Add first operand if needed (non-register and non-empty) */
	if (operand1.type != REGISTER && operand1.type != NONE)
	{
		addBinaryOperand(operand1);
		IC++; /* increment instruction counter */
	}
	/* Add second operand if needed (non-register and non-empty) */
	if (operand2.type != REGISTER && operand2.type != NONE)
	{
		addBinaryOperand(operand2);
		IC++;
	}
}

void buildCodeData(int *integers, int num_of_int)
{
	int i; /* index for loops */
	/* Add each integer as a data word */
	for (i = 0; i < num_of_int; i++)
	{
		addDataCode(integers[i]);
		IC++; /* increment instruction counter */
		DC++; /* increment data counter */
	}
}

void buildCodeString(char *string, int num_of_nodes)
{
	int i; /* index for loops */
	/* Start from index 1 to skip the opening quotation mark */
	for (i = 1; i < num_of_nodes; i++)
	{
		addAsciiCode((int)string[i]);
		IC++;
		DC++;
	}
	/* Add null terminator at the end of the string */
	addAsciiCode(0);
	IC++;
	DC++;
}

void addAsciiCode(int node)
{
	memory_unit new_memory; /* structure representing a memory cell */

	new_memory.num = IC;					/* set the memory unit address to current instruction counter */
	strcpy(new_memory.content, "");			/* Clears symbol content field */
	new_memory.adress = NONE;				/* no addressing method for charecter */
	new_memory.code.full_code.value = node; /* store the ASCII value */
	addNewMemory(new_memory);				/* insert the memory unit into memory */
}

void addDataCode(int integer)
{
	memory_unit new_memory; /* structure representing a memory cell */

	new_memory.num = IC;					   /* set the memory unit address to current instruction counter */
	strcpy(new_memory.content, "");			   /* Clears symbol content field */
	new_memory.adress = NONE;				   /* no addressing method for .data values */
	new_memory.code.full_code.value = integer; /* store the integer value */
	addNewMemory(new_memory);				   /* insert the memory unit into memory */
}

void addBinaryOpcode(operand operand1, operand operand2, int opcode, int opcode_funct)
{
	int source_register_num; /* Holds source register number if source operand is a register */
	int dest_register_num;	 /* Holds destination register number if destination operand is a register */
	memory_unit new_memory;	 /* Struct representing a single memory cell */

	new_memory.adress = OPCODE;
	strcpy(new_memory.content, ""); /* Clears symbol content field */
	new_memory.num = IC;			/* Assigns current instruction counter as memory address */
	new_memory.code.main_code.E = 0;
	new_memory.code.main_code.R = 0;
	new_memory.code.main_code.A = 1;
	new_memory.code.main_code.funct = opcode_funct;
	new_memory.code.main_code.opcode_num = opcode;

	/* Instruction with no operands */
	if (operand1.type == NONE && operand2.type == NONE)
	{
		new_memory.code.main_code.source_reg = 0;
		new_memory.code.main_code.source_adress = 0;
		new_memory.code.main_code.dest_reg = 0;
		new_memory.code.main_code.dest_adress = 0;
	}
	/* if there is 2 operands */
	if (operand2.type != NONE)
	{
		if (operand1.type == REGISTER)
		{
			source_register_num = returnReg(operand1.content);
			new_memory.code.main_code.source_reg = source_register_num;
			new_memory.code.main_code.source_adress = THREE;
		}
		if (operand1.type == DIRECT)
		{
			new_memory.code.main_code.source_reg = 0;
			new_memory.code.main_code.source_adress = 1;
		}
		if (operand1.type == IMMEDIATE)
		{
			new_memory.code.main_code.source_reg = 0;
			new_memory.code.main_code.source_adress = 0;
		}
		if (operand1.type == RELATIVE)
		{
			new_memory.code.main_code.source_reg = 0;
			new_memory.code.main_code.source_adress = TWO;
		}
		if (operand2.type == REGISTER)
		{
			dest_register_num = returnReg(operand2.content);
			new_memory.code.main_code.dest_reg = dest_register_num;
			new_memory.code.main_code.dest_adress = THREE;
		}
		if (operand2.type == DIRECT)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = 1;
		}
		if (operand2.type == IMMEDIATE)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = 0;
		}
		if (operand2.type == RELATIVE)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = TWO;
		}
	}
	/* if there is only 1 operand */
	else
	{
		new_memory.code.main_code.source_reg = 0;
		new_memory.code.main_code.source_adress = 0;
		if (operand1.type == REGISTER)
		{
			dest_register_num = returnReg(operand1.content);
			new_memory.code.main_code.dest_reg = dest_register_num;
			new_memory.code.main_code.dest_adress = THREE;
		}
		if (operand1.type == DIRECT)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = 1;
		}
		if (operand1.type == IMMEDIATE)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = 0;
		}
		if (operand1.type == RELATIVE)
		{
			new_memory.code.main_code.dest_reg = 0;
			new_memory.code.main_code.dest_adress = TWO;
		}
	}
	addNewMemory(new_memory); /* Adds the constructed machine code to memory */
}

void addBinaryOperand(operand ope)
{
	int code;						/* the value of the 3-23 bits */
	memory_unit new_memory;			/* Struct representing a single memory cell */
	new_memory.num = IC;			/* Assign the current instruction counter */
	strcpy(new_memory.content, ""); /* Clears symbol content field */
	if (ope.type == IMMEDIATE)
	{
		new_memory.adress = IMMEDIATE;			 /* Addressing type is immediate */
		new_memory.code.value_code.E = 0;		 /* External flag */
		new_memory.code.value_code.R = 0;		 /* Relocatable flag */
		new_memory.code.value_code.A = 1;		 /* Absolute flag */
		code = atoi(ope.content);				 /* Convert operand content to integer */
		new_memory.code.value_code.value = code; /* Store the value */
	}
	else
	{
		if (ope.type == DIRECT)
		{
			strcpy(new_memory.content, ope.content); /* Save the symbol name */
			new_memory.adress = DIRECT;				 /* Addressing type is direct */
			new_memory.code.value_code.E = 0;		 /* External flag */
			new_memory.code.value_code.R = 1;		 /* Relocatable flag */
			new_memory.code.value_code.A = 0;		 /* Absolute flag */
			new_memory.code.value_code.value = 0;	 /* temporery value - resolved later(in the second pass) */
		}
		if (ope.type == RELATIVE)
		{
			strcpy(new_memory.content, ope.content); /* Save the symbol name */
			new_memory.adress = RELATIVE;			 /* Addressing type is relative */
			new_memory.code.value_code.E = 0;		 /* External flag */
			new_memory.code.value_code.R = 0;		 /* Relocatable flag */
			new_memory.code.value_code.A = 1;		 /* Absolute flag */
			new_memory.code.value_code.value = 0;	 /* temporery value - resolved later(in the second pass) */
		}
	}
	addNewMemory(new_memory); /* Adds the constructed machine code to memory */
}

void addNewMemory(memory_unit new_unit)
{
	/* Check if there's space in the memory array */
	if (memory_count < MEMORY_SIZE)
	{
		memory[memory_count] = new_unit; /* Add the new memory unit */
		memory_count++;					 /* Move to next free memory slot */
	}
	else
	{
		printf("Memory array is full!\n");
		exit(0);
	}
}

void clearMemory()
{
	memset(memory, 0, sizeof(memory_unit) * MEMORY_SIZE);
	memory_count = 0; /* initiallize the number of used memory back to 0 */
}

void memory_translate(memory_unit unit, FILE *obFile)
{
	int binary_value = 0; /* Holds the final binary value to be printed */

	if (unit.adress == OPCODE)
	{
		/* Place each field at the correct bit position (E is bit 0) */
		/* Main opcode format (24-bit):
		 * Bits 0-2   : ARE (E, R, A)
		 * Bits 3-7   : Function (funct)
		 * Bits 8-10  : Destination register
		 * Bits 11-12 : Destination addressing
		 * Bits 13-15 : Source register
		 * Bits 16-17 : Source addressing
		 * Bits 18-23 : Opcode number
		 */
		binary_value |= unit.code.main_code.E << 0;
		binary_value |= unit.code.main_code.R << 1;
		binary_value |= unit.code.main_code.A << TWO;
		binary_value |= unit.code.main_code.funct << THREE;			  /* bits 3–7*/
		binary_value |= unit.code.main_code.dest_reg << EIGHT;		  /* bits 8–10*/
		binary_value |= unit.code.main_code.dest_adress << ELEVEN;	  /* bits 11–12*/
		binary_value |= unit.code.main_code.source_reg << THIRTEEN;	  /* bits 13–15*/
		binary_value |= unit.code.main_code.source_adress << SIXTEEN; /* bits 16–17*/
		binary_value |= unit.code.main_code.opcode_num << EIGHTEEN;	  /* bits 18–23*/
	}
	else if (unit.adress == IMMEDIATE || unit.adress == DIRECT || unit.adress == RELATIVE)
	{
		/* value_binary: E (bit 0), R (bit 1), A (bit 2), value (bits 3–23) */
		binary_value |= unit.code.value_code.E << 0;
		binary_value |= unit.code.value_code.R << 1;
		binary_value |= unit.code.value_code.A << TWO;
		binary_value |= unit.code.value_code.value << THREE; /*value is 21 bits*/
	}
	else
	{
		/* full_binary: entire 24-bit value stored directly */
		binary_value = unit.code.full_code.value; /* assume bits already correct */
	}
	/* Print hexadecimal representations in the .ob file */
	print_hexa(binary_value, obFile, unit.num);
}

void print_hexa(int binary_value, FILE *obFile, int num)
{
	char hexString[HEXA_SIZE];		   /* Buffer to hold the hex string (6 digits + null terminator) */
	char decimal_adress[DECIMAL_SIZE]; /* Buffer to hold the decimal address */
	int i;							   /* index for loops */
	int bitLeft;					   /* Position to shift bits to extract each 4-bit segment */
	int hexaValue;					   /* Holds value of the current 4-bit segment (0-15) */

	binary_value = binary_value & BITS_MASK; /* Ensures we only use the lowest 24 bits */

	decimalToPrint(num, decimal_adress); /* Convert num(place in the memory) to the decimal address */

	for (i = 0; i < SIX; i++)
	{
		bitLeft = FOUR * (FIVE - i);					 /* Calculate how much to shift to get the i-th segment */
		hexaValue = (binary_value >> bitLeft) % SIXTEEN; /* Extract 4 bits */

		if (hexaValue < TEN)
		{
			hexString[i] = '0' + hexaValue; /* Convert 0–9 to ASCII */
		}
		else
		{
			hexString[i] = 'a' + (hexaValue - TEN); /* Convert 10–15 to 'a'–'f' */
		}
	}

	hexString[SIX] = '\0';
	fprintf(obFile, "%s\t%s\n", decimal_adress, hexString); /* Print address and hex value to the .ob file */
}

void binaryForDirect(symbol curSymbol)
{
	int i;								/* index for loops */
	int memory_value = curSymbol.value; /* Holds the final memory value assigned to the symbol */

	for (i = 0; i < memory_count; i++)
	{
		if (!strcmp(memory[i].content, curSymbol.name)) /* Look for memory units with the symbol name in the content field */
		{
			memory[i].code.value_code.value = memory_value; /* Set the correct memory address */
			strcpy(memory[i].content, "");					/* Clear the symbol content after use */
			return;
		}
	}
	return;
}

void binaryForRelative(char *symbolName, int symbolValue)
{
	int i; /* index for loops */
	for (i = 0; i < memory_count; i++)
	{
		if (!strcmp(memory[i].content, symbolName)) /* Look for memory units with the symbol name in the content field */
		{
			memory[i].code.value_code.value = symbolValue - memory[i].num + 1; /* Calculate relative address(distance) and assign it */
			strcpy(memory[i].content, "");									   /* Clear the symbol content after use */
			return;
		}
	}
	return;
}

void binaryForExternal(symbol curSymbol, int errors, FILE *extFile)
{
	int memory_value;									 /* holds the memory address of the external symbol */
	char decimal_adress[DECIMAL_SIZE];					 /* for the decimal adress */
	memory_value = changeMemoryExternal(curSymbol.name); /* get address where symbol is used */
	decimalToPrint(memory_value, decimal_adress);
	/* not printing on the .ext file if there is error */
	if (errors == 0)
	{
		fprintf(extFile, "%s\t%s\n", curSymbol.name, decimal_adress); /* write symbol name and address to the .ext file */
	}
}

int changeMemoryExternal(char *curSymbol)
{
	int i;		   /* index for loops */
	int value = 0; /* default return value */
	for (i = 0; i < memory_count; i++)
	{
		if (!strcmp(memory[i].content, curSymbol))
		{
			value = memory[i].num;			 /* save memory address */
			strcpy(memory[i].content, "");	 /* clear symbol name field */
			memory[i].code.value_code.E = 1; /* mark as external */
			memory[i].code.value_code.R = 0; /* reset R bit */
			return value;
		}
	}
	return value;
}

void runObjectFile(FILE *obFile)
{
	int i;											/* index for loops */
	int numOfinst = IC - MIN_IC - DC;				/* number of instruction words (excluding data) */
	fprintf(obFile, "     %d %d\n", numOfinst, DC); /* write header line with code and data sizes */

	for (i = 0; i < memory_count; i++)
	{
		memory_translate(memory[i], obFile); /* translate each memory unit and write to .ob file */
	}
}

