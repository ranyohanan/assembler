/*
 * assembler.c
 * This file contains the main entry point of the assembler.
 * It applies the macro expansion, first pass, and second pass for each file.
 */
#include "assembler.h"

int IC = 100; /* Instruction Counter */
int DC = 0;   /* Data Counter */
int main(int argc, char *argv[])
{
    int i;
    /* when no file was sent */
    if (argc == 1)
    {
        printf("\nNo file has been given\n");
        exit(0);
    }

    for (i = 1; i < argc; i++)
    {
        /* the mcros deployment */
        pre_as(argv[i]);

        /* first pass - analyse the code and build symbol table and part of the memory */
        if (firstPass(argv[i]) != 0)
        {
            /* if there is an error in the first pass - cleans memory and skip to next file */
            free_mcros();
            free_symbols();
            continue;
        }
        /* Second pass - complete the memory picture and the output files */
        secondPass(argv[i]);
        /* clean memory before the next file */
        free_mcros();
        free_symbols();
    }

    return 0;
}

void pre_as(char *fileName)
{
    mcroHandle(fileName);
}

int firstPass(char *fileName)
{
    return firstPassHandle(fileName);
}

void secondPass(char *fileName)
{
    secPassHandle(fileName);
}

