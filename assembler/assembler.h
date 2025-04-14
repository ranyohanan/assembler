#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "binary.h"
#include "mcros.h"
#include "symbols.h"
#include "as_to_am.h"
#include "am_first.h"
#include "am_sec.h"

/* function for starting the first step of the assembler - the mcros deployment */
void pre_as(char *);

/* Executes the first pass of the assembler */
int firstPass(char *);

/* Executes the second pass of the assembler and generates final output files */
void secondPass(char *fileName);
#endif

