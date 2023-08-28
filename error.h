#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#define FATAL_ERROR 1
#define INVALID -1

#define INVALID_ARGUMENTS "ERROR: No arguments given to assembler\nFormat: ./assembler arg1,...,argn\n"
#define CANT_READ_FILE "ERROR: File does not exist / error while opening\n"
#define MEMORY_ERROR "ERROR: Invalid memory allocation\n"
#define INVALID_ENDMCRO "ERROR: Invalid endmcro declaration"
#define INVALID_MCRO "ERROR: Invalid mcro declaration"
#define INVALID_LABEL "ERROR: Invalid label name"
#define INVALID_STRING "ERROR: Invalid string declaration"
#define INVALID_DATA "ERROR: Invalid data declaration"
#define SYMBOL_ALREDY_EXISTS "ERROR: Symbol alredy exists"
#define UNNECESSARY_SYMBOL "WARNING: Ignored symbol"
#define INVALID_KEYWORD "ERROR: Invalid keyword"
#define INVALID_OPERANDS "ERROR: Invalid number of operands"
#define INVALID_COMMAND "ERROR: Invalid command"
#define INVALID_COMMAS "ERROR: Invalid number of commas"
#define EMPTY_LABEL "ERROR: Empty label"
#define INVALID_SOURCE_OPERAND "ERROR: Unidentified source operand"
#define INVALID_DEST_OPERAND "ERROR: Unidentified desination operand"
#define MEMORY_OVERFLOW "ERROR: Memory overflow"
#define INVALID_OPERAND_COUNT "ERROR: Invalid operand count"
#define OVERFLOW_DATA_OPERAND "ERROR: Data operand cannot fit in 12 bits"
#define ENTRY_DEFINED_AS_EXTERN "ERROR: Invalid entry operand is alredy extern"
#define ENTRY_NOT_FOUND "ERROR: entry is not in label table"
#define INVALID_QUOTES "ERROR: Invalid quotes"

/*
 * Raises an error and increments the error count.
 */
void raise_error(const char*);

/*
 * Raises an error with line number information and increments the error count.
 */
void raise_error_in_line(const char*, int);

/*
 * Raises a warning with line number information.
 */
void raise_warning_in_line(const char*, int);

/*
 * Checks if a given word is a valid keyword in the assembly language.
 * Returns TRUE (1) if it is a keyword, FALSE (0) otherwise.
 */
int error_invalid_keyword(char*);

/*
 * Checks the validity of an instruction line based on command and operands.
 */
void error_check_instruction(char*, int, char**, int);

/*
 * Checks if the number of commas in a line matches the expected count.
 * Also ensures there are no consecutive commas.
 * Returns TRUE (1) if comma count is correct, FALSE (0) otherwise.
 */
int error_check_commas(char*, int);

#endif