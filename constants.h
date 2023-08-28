#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
* This file holds all the defines used throughout the program 
*/

#define TABLE_BASE_SIZE 20
#define UPDATE_SIZE(x) (x + TABLE_BASE_SIZE)
#define MAX_LINE_LENGTH 82
#define MAX_TOKENS 20

#define TRUE 1
#define FALSE 0
#define INVALID -1

#define KEYWORDS_COUNT 28

#define IC_TYPE 0
#define DC_TYPE 1
#define EXTERN_TYPE 2
#define ENTRY_TYPE 3

#define MAX_LABEL_LENGTH 32
#define AMOUNT_OF_COMMANDS 16
#define AMOUNT_OF_REGISTERS 8

#define MAX_OPERANDS 2
#define WORD_SIZE 12
#define MEMORY_SIZE 924

#define MAX_NUM_OPERAND 511
#define MIN_NUM_OPERAND -512

#define MAX_DATA_OPERAND 2047
#define MIN_DATA_OPERAND -2048

#define MEMORY_OFFSET 100

#define REGISTER 5
#define LABEL 3
#define ABSOLUTE 1

#endif