#ifndef AM_FIRST_H
#define AM_FIRST_H

#include "analyser.h"
#include "mcros.h"
#include "symbols.h"
#include "binary.h"

/* Performs the first pass on a given file. and Returns the number of errors found */
int firstPassHandle(char *fileName);
/* Handles a line that includes both a symbol and a guide word. */
void handleSymbolGuide(char *line, char *symbol, char *guide_word, char *amFileName, int num_of_line, int num_of_words);
/* Handles a guide word line (without symbol definition). */
void handleGuide(char *line, char *guide_word, char *amFileName, int num_of_line, int num_of_words);
/* Handles a line that includes both a symbol and an opcode. */
void handleSymbolOpcode(char *line, char *symbol, char *opcode, char *amFileName, int num_of_line, int num_of_words);
/* Handles an opcode line (without symbol definition). */
void handleOpcode(char *line, char *opcode, char *amFileName, int num_of_line, int num_of_words);
/* Sets and classifies the operand's addressing type and value. */
operand setOperand(char *line, char *symbol, char *amFileName, int num_of_line);

#endif
