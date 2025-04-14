#ifndef AM_SEC_H
#define AM_SEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "analyser.h"
#include "symbols.h"
#include "binary.h"

/* Performs the second pass of the assembler over the .am file */
void secPassHandle(char *fileName);
/* Handles the .entry guide word: marks symbol as entry if valid */
boolean handleEntrySymbol(char *line, int num_of_line, char *amFileName);
/* Resolves direct or external symbols for binary translation */
int symbolFound(char *wth, int errors, FILE *extFile);
/* Resolves relative addressing symbols, checks for extern conflicts */
boolean relativeSymbolFound(char *symbol, int errors, char *amFileName, int num_of_line);

#endif

