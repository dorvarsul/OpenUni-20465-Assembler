#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>

/*
* This file holds the public global variables used throughout the program
*/


int errors; /* Number of errors detected in each one of the assembler phases*/
int dc; /* Data counter */
int ic; /* Instruction counter */
int line_num; /* Current line number we are processing */

#endif