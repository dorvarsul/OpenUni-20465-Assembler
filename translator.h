#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

/*
* Translates a word of binary into base64 format and writes it to output file
*/
void translator_translate_word(FILE*, char*);

/*
* Encodes an instruction operand into its binary representation
*/
void translator_encode_operand(char*, char*, int, FILE*, int);

/*
* Encodes a data operand into its binary representation
*/
void translator_encode_data(char*, int);

/*
* Encodes a char operand into its binary representation
*/
void translator_encode_char(char*, char);
#endif