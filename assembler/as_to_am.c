#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> 
#include "analyser.h"
#include "mcros.h"
#include "as_to_am.h"

static int contentCount = 0; /* number of lines inside a mcro content*/

void mcroHandle(char *fileName)
{
    char asFileName[WORD_MAX_SIZE]; /*a string for as file name*/
    char amFileName[WORD_MAX_SIZE]; /*a string for am file name*/
    char line[MAX_LINE]; /*an array to store current line*/
    char lineToPrint[MAX_LINE]; /*an array to store current line and write it in the am file if its regular line*/
    char *word; /*pointer to store current word*/
    boolean length; /*a boolean to indicate if the line length is legal*/
    int potMcro; /*flag to indicate a potential mcro*/
    int word_place = 0;   /*tell the place of the word in the line*/
    int num_of_words = 0; /*the number of words in the line*/
    int quotation_count;  /*tell the number of quotation marks in the line*/
    int num_of_line = 0;  /*the current number of line*/
    int contentToAdd; /*flag to indicate if there is a content to add to new mcro*/
    char *mcroContent = NULL; /*pointer to store new mcro's content*/
    char *p;   /*pointer to node in a line*/
    FILE *asf; /*pointer for as file*/
    FILE *amf; /*pointer for am file*/
    fpos_t cur; /*pointer for the first node in line of the as file*/
    p = line; 

    strcpy(asFileName, fileName);
    strcpy(amFileName, fileName);
    strcat(asFileName, ".as");
    strcat(amFileName, ".am");

    asf = fopen(asFileName, "r+");
    if (!asf)
    {
        printf("\n%s File has failed to open\n", asFileName);
        return;
    }
    amf = fopen(amFileName, "w");
    if (!amf)
    {
        printf("\n%s File has failed to open\n", amFileName);
        return;
    }

    fgetpos(asf, &cur);
    /*handle a line in the file each time*/
    while (fgets(line, sizeof(line), asf))
    {
        num_of_line++;
        fsetpos(asf, &cur);
        fgets(lineToPrint, sizeof(lineToPrint), asf);
        fsetpos(asf, &cur);
        /* check line length */
        length = lineLength(asf);
        /* when line length is illegal */
        if (length == FALSE)
        {
            printf("\nError in line %d: Line length is bigger then expected\n", num_of_line);
            free(mcroContent);
            fclose(asf);
            fclose(amf);
            remove(amFileName);
            return;
        }
        fgetpos(asf, &cur);
        p = line;
        word_place = 0;
        quotation_count = 0;
        potMcro = OFF;
        contentToAdd = OFF;
        /* counting the number of words in the line */
        num_of_words = wordCounter(p);
        /* handle each word in line */
        word = strtok(line, " \t\n");
        while (word != NULL)
        {
            word_place++;
            if (!strcmp(word, "\""))
            {
                quotation_count++;
            }
            /* make a check if the word is "mcro" and act accordingly */
            if (!strcmp(word, "mcro"))
            {
                if (num_of_words == TWO && word_place == 1)
                {
                    potMcro = ON;
                    word = strtok(NULL, " \t\n");
                    continue;
                }
                else if (quotation_count % TWO == 1)
                {
                    word = strtok(NULL, " \t\n");
                    continue;
                }
                else
                {
                    printf("\nError in line %d: Mcro hasn't set correctly\n", num_of_line);
                    if (mcroContent != NULL)
                    {
                        free(mcroContent);
                    }
                    fclose(asf);
                    fclose(amf);
                    remove(amFileName);
                    return;
                }
            }
            /* when mcro set correctly and there is a potential new mcro to add */ 
            if (potMcro == ON)
            {
                if (checkNewMcro(word, strlen(word)) == TRUE && checkExistMcro(word) == FALSE)
                {
                    contentCount = 0;
                    /* start to collect the content of the new mcro */
                    mcroContent = collectContent(asf, num_of_line);
                    fgetpos(asf,&cur);
                    if (mcroContent != NULL)
                    {
                        num_of_line += contentCount;
                        /* alerting that the buffer of the content is ready to be added to the new mcro */
                        contentToAdd = ON;
                        break;
                    }
                    else
                    {
                        fclose(asf);
                        fclose(amf);
                        remove(amFileName);
                        return;
                    }
                }
                else
                {
                    printf("\nError in line %d: Mcro name hasn't set correctly\n", num_of_line);
                    fclose(asf);
                    fclose(amf);
                    remove(amFileName);
                    return;
                }
            }
            /* make a check if the word is an already existing mcro and act accordingly */
            if (num_of_words == 1)
            {
                if (checkExistMcro(word) == TRUE)
                {
                    mcroContent = findMcro(word);
                    writeLine(mcroContent, amf);
                    break;
                }
            }
            word = strtok(NULL, " \t\n");
        }
        /* after collecting all the content of the new mcro */
        if (contentToAdd == ON)
        {
            /* add the new mcro with the new name and content*/
            addNewMcro(mcroContent, word);
            free(mcroContent);
            contentToAdd = OFF;
        }
        else
        {
            /* when its a regular and legal line */
            if (num_of_words > 0 && word == NULL)
            {
                writeLine(lineToPrint, amf);
            }
        }
    }
    fclose(asf);
    fclose(amf);
    return;
}


char *collectContent(FILE *file, int num_of_line)
{
    char contentLine[MAX_LINE]; /*an array to store current line*/
    char contentToBuf[MAX_LINE]; /*an array to store current line that supposed to be sent to the buffer*/
    size_t buffer_size; /*the buffer size for reallocation*/
    size_t buffer_length; /*current buffer length*/
    size_t line_length; /*current line length*/
    int end; /*flag to indicate if its the end of the mcro content*/
    char *buffer; /*pointer for a buffer to all collected content*/
    char *word;   /*pointer for a one word in a line*/
    char *tempBuf; /*a temp pointer for a safe reallocation*/
    fpos_t cur; /*pointer for am file*/
    buffer_size = MAX_LINE * TWO;
    buffer_length = 0;
    end = OFF;

    buffer = malloc(buffer_size);
    if (!buffer)
    {
        printf("faild to allocate memory\n");
        exit(0);
    }
    buffer[0] = '\0';

    fgetpos(file, &cur);
    do
    {
        fgets(contentLine, sizeof(contentLine), file);
        fsetpos(file, &cur);
        fgets(contentToBuf, sizeof(contentToBuf), file);
        contentCount++;
        /* if the current line length is illegal */
        if (strlen(contentLine) > MAX_LINE)
        {
            free(buffer);
            printf("\nError in line %d: Line length is bigger then expected\n", num_of_line + contentCount);
            return NULL;
        }
        fgetpos(file, &cur);
        /* to prevent an empty lines */
        if (wordCounter(contentLine) > 0)
        {
            word = strtok(contentLine, " \t\n");
            while (word != NULL)
            {
            	/* when spoted a mcroend */
                if (!strcmp(word, "mcroend"))
                {
                    end = ON;
                    break;
                }
                /* when an already existed mcro appears in a mcro content */
                if (checkExistMcro(word) == TRUE)
                {
                    printf("\nError in line %d: Cant use an existed mcro inside a new mcro\n", num_of_line + contentCount);
                    return NULL;
                }
                word = strtok(NULL, " \t\n");
            }
            /* adding the line to the mcro's content buffer */
            if (end == OFF)
            {
                line_length = strlen(contentToBuf);
                if (buffer_length + line_length + 1 > buffer_size)
                {
                    buffer_size += MAX_LINE * TWO;
                    tempBuf = realloc(buffer, buffer_size);
                    if (!tempBuf)
                    {
                        free(buffer);
                        printf("faild to allocate memory\n");
                        exit(0);
                    }
                    buffer = tempBuf;
                }
                strcat(buffer, contentToBuf);
                buffer_length += line_length;
            }
        }

    } while (end == OFF);
    /* when the mcro is empty */
    if(buffer[0] == '\0')
    {	
    	printf("\nError in line %d: Mcro has no content\n", num_of_line);
    	free(buffer);
    	return NULL;
    }
    /* returning the new mcro's content */
    if (end == ON && wordCounter(contentLine) == 1)
    {
        return buffer;
    }
    free(buffer);
    return NULL;
}

void writeLine(char *content, FILE *amFile)
{
    size_t size = strlen(content); /* length of the line to write */
    if (size > 0 && content[size - 1] == '\n')
    {
        fprintf(amFile, "%s", content);
    }
    else
    {
        fprintf(amFile, "%s\n", content);
    }
}

boolean lineLength(FILE *file)
{
    int ch; /* the current node */
    int counter = 0; /* counter of the nodes in the line*/
    while ((ch = fgetc(file)) != '\n')
    {
        counter++;
    }
    if (counter > MAX_LINE - 1)
    {
        return FALSE;
    }
    return TRUE;
}
