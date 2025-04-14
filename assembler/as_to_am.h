#ifndef AS_TO_AM_H
#define AS_TO_AM_H

#include <stdio.h>
#include "analyser.h"

/* function for indicate if the length of the line in the file is legal*/
boolean lineLength(FILE*);

/* function for writing a line or content inside a file */
void writeLine(char*, FILE*);

/* function for collecting all the content of a potential mcro */
char* collectContent(FILE*,int);

/* the main function of handling mcros in the as file and deploy them in the am file */
void mcroHandle(char*);


#endif
