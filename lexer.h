#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>

/*
* This function takes in a line from the input file, analyzes its operands
* and encodes it to binary representation
*/
void lexer_analyze_operation(char**, int, int, int, FILE*);

/*
* This function encodes data into binary representation and appends it
* to the data image
*/
void lexer_analyze_data(char**, int, int);

/*
* This function recieves a string , analyzes it and encodes it into binary_functio
*/
void lexer_analyze_string(char*);

#endif