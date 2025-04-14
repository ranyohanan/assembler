#ifndef MCROS_H
#define MCROS_H

#include "analyser.h"

/* a struct for a mcro */
typedef struct
{
	char name[WORD_MAX_SIZE];
	char *content;
}mcro;


/* function for adding a new mcro */
void addNewMcro(char*, char*);

/* function for locate an exist mcro by his name */
char* findMcro(char*);

/* a function to check if a word is a mcro name */
boolean checkExistMcro(char*);

/* a function for free all mcros */
void free_mcros();

#endif
