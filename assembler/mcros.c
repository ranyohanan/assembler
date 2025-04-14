#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "analyser.h"
#include "mcros.h"

static int mcro_count = 0; /* counter for the number of mcros */
static mcro *mcros = NULL; /* pointer for all the mcros of the file */

void addNewMcro(char *newContent, char *mcroName)
{
    mcro *temp; /* temporary pointer for safer reallocation*/
    
    /* increasing the storage */
    if (mcros == NULL)
    {
        mcros = malloc(sizeof(mcro));
        if (!mcros)
        {
            printf("\nFailed to allocate memory for mcros\n");
            exit(0);
        }
    }
    else
    {
        temp = realloc(mcros, (mcro_count+1) * sizeof(mcro));
        if (!temp)
        {
            printf("\nfaild to reallocate memory\n");
            exit(0);
        }
        mcros = temp; 
    }
    mcro_count++; /* increasing the count of mcros */
    strcpy(mcros[mcro_count - 1].name, mcroName); /* save the mcro name */

    mcros[mcro_count - 1].content = malloc(strlen(newContent) + 1); /* build the storage of mcro's content */
    if (!mcros[mcro_count - 1].content)
    {
        printf("\nfaild to allocate memory\n");
        exit(0);
    }
    
    strcpy(mcros[mcro_count - 1].content, newContent); /* save the mcro content */
}

char* findMcro(char *mcroName)
{
    int i; /* index for loop */
    /* finding the content of the mcro by the mcro name */ 
    for (i = 0; i < mcro_count; i++)
    {
        if (!strcmp(mcros[i].name, mcroName))
        {
            return mcros[i].content;
        }
    }
    return NULL;
}

boolean checkExistMcro(char *word)
{
    int i; /* index for loop */
    /* checking if the mcro exist by the mcro name */
    if (mcros != NULL)
    {
        for (i = 0; i < mcro_count; i++)
        {
            if (!strcmp(mcros[i].name, word))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void free_mcros()
{
    int i; /* index for loop */
    /* free all the mcros content */
    if(mcros != NULL){
    	for (i = 0; i < mcro_count; i++)
    	{	
        	free(mcros[i].content);
    	}
    	/* free all mcros */
    	free(mcros);
    	mcros = NULL;
    	mcro_count = 0;
    }
}
