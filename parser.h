#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
* This function is responsible for pre-assembling the file, spreading macros and
* ignore empty lines and comments and write .am file 
*/
int parser_assemble_file(FILE*, char*);

/*
* This function is responsible for initial error checking, making sure the
* file can then be passed on to initial translation
*/	
void parser_first_pass(FILE*);

/*
* This function is responsible for more complex error checking, and initial
* translation
*/
int parser_second_pass(FILE*, char*);




#endif