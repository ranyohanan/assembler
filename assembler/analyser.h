#ifndef ANALYSER_H
#define ANALYSER_H

#define WORD_MAX_SIZE 31  /* maximum length of a legal mcro name */
#define NUM_OPCODES 16	  /* the number of opcodes */
#define NUM_REGISTERS 8	  /* the number of registers */
#define MAX_LINE 80		  /* maximum length of a legal line */
#define GUIDE_WORD_SIZE 6 /* maximum length of a guide word */
/* Numeric constants used in various calculations */
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define NINE 9
#define TEN 10
#define TWELVE 12
#define THIRTEEN 13
#define FOURTEEN 14
#define FIFTEEN 15

#define DECIMAL_SIZE 8 /* Length of string to hold a decimal address as string (7 digits + null) */
#define MIN_IC 100	   /* Initial value of the instruction counter */

/* Global counters for instruction and data memory */
extern int IC;
extern int DC;

enum
{
	OFF,
	ON
}; /* enum for turn on and off flags */

typedef enum
{
	FALSE,
	TRUE
} boolean; /* boolean typedef for functions and conditions */

typedef enum
{
	NONE,
	IMMEDIATE,
	DIRECT,
	RELATIVE,
	REGISTER,
	OPCODE
} adress_type; /* enum typedef to define the adressing type of the operand */

typedef struct
{
	char content[WORD_MAX_SIZE];
	adress_type type;
} operand; /* struct for operand */

typedef struct
{
	char name[FOUR];
	int funct;
	int code;
	adress_type source_op[THREE];
	adress_type dest_op[THREE];
} opcode; /* struct for all possible opcodes */

/* Returns number of quotation marks in a line */
int quotation(char *);

/* Returns number of words in a line (basic) */
int wordCounter(char *);

/* Return TRUE if the last node in the line is "," */
boolean lastIsComma(char *line);

/* Returns number of words in a line (specific for .am file) */
int wordCounter_for_am(char *p);

/* Removes leading spaces from 'p' and copies result into 'line' */
void removeLeadingSpaces(char *p, char *line);

/* function return true if the new potential mcro name is legal */
boolean checkNewMcro(char *, int);

/* Returns TRUE if the line contains two or more consecutive commas */
boolean doubleCommas(char *line);

/* Returns TRUE if the line contains a colon (symbol definition) */
boolean findColon(char *line);

/* Returns TRUE if a given word is a valid guide word (e.g., .data) */
boolean checkGuide(char *guide);

/* Returns TRUE if a given word is a legal symbol name */
boolean checkSymbol(char *symbol, int num_of_line, char *amFileName);

/* Returns TRUE if a symbol is a potential symbol */
boolean potSymbol(char *symbol);

/* Returns TRUE if the word is a legal register (r0–r7) */
boolean isRegister(char *word);

/* Returns TRUE if the word matches an opcode name */
boolean isOpcode(char *word);

/* Returns TRUE if word is a valid operand (immediate, symbol, register...) */
boolean checkOperand(char *word);

/* Returns TRUE if word represents a valid integer number */
boolean checkNum(char *word);

/* Converts a number into a 7-digit zero-padded string */
void decimalToPrint(int value, char *decimal_adress);

/* Returns TRUE if string is wrapped in quotation marks */
boolean checkString(char *string);

/* Returns TRUE if first operand is valid as a source/destination operand for the opcode */
boolean operand1ToOpcode(operand operand1, operand operand2, char *opcode);

/* Returns TRUE if second operand is valid as a destination operand for the opcode */
boolean operand2ToOpcode(operand operand, char *opcode);

/* Returns the opcode number (code) for a given opcode name */
int returnOpcodeNum(char *opcode);

/* Returns the funct value for a given opcode name */
int returnFunct(char *opcode);

/* Returns the register number (0–7) for a given register name */
int returnReg(char *reg);

#endif

