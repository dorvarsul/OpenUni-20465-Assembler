#include "error.h"
#include "globals.h"
#include "constants.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>
	

/*
 * Raises an error and increments the error count.
 */
void raise_error(const char* message){
	errors++;
	fprintf(stderr, "%s\n", message);
}

/*
 * Raises an error with line number information and increments the error count.
 */
void raise_error_in_line(const char* message, int line_num) {
	fprintf(stderr, "%s at line: %d\n", message, line_num);
    errors++;
}

/*
 * Raises a warning with line number information.
 */
void raise_warning_in_line(const char* message, int line_num) {
    fprintf(stderr, "%s at line: %d\n", message, line_num);
}

/* Recieves a string and returns TRUE (1) if it is one of the saved keywords in the assembly language
	returns FALSE (0) otherwise.*/
int error_invalid_keyword(char* word) {
	
	const char* keywords[] = {
	"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",
	"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
	"dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
	".entry", ".data", ".string", ".extern"
	};
	
	int i;
	
	if (!isalpha(word[0])){
        raise_error_in_line(INVALID_KEYWORD, line_num);
        errors++;
    }
	
	for(i=0; i < KEYWORDS_COUNT; i++) {
		if (!strcmp(word, keywords[i])) {
			return TRUE;
		}
	}
	return FALSE;
}

void error_check_instruction(char* line, int is_symbol, char** tokens, int token_count) {
	/* A constant table of the known commands */
	const char* keywords[] = {
	"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
	"dec", "jmp", "bne", "red", "prn","jsr", "rts", "stop"
	};
	
	int i; /* Counter */
	int command_type = INVALID; /* Type of command, -1 stands for INVALID */
	int expected_commas = 0;
	
	for(i=0; i < AMOUNT_OF_COMMANDS; i++) {
		if(!strcmp(tokens[is_symbol], keywords[i])) {
			command_type = i;
			break;
		}
	}
	
	if (command_type == -1) {
		raise_error_in_line(INVALID_COMMAND, line_num);
		return;
	}
	
	/* All commands with 2 operands */
	if(command_type <= 3 || command_type == 6) {
		if (token_count != (is_symbol + 3)) {
			raise_error_in_line(INVALID_OPERANDS, line_num);
			return;
		}
		
		expected_commas = 1;
	}
	
	/* All commands with 1 operand */
	if(command_type == 4 || command_type == 5 || (command_type >=  7 && command_type <= 13)) {
		if(token_count != (is_symbol + 2)) {
			raise_error_in_line(INVALID_OPERANDS, line_num);
			return;
		}
	}
	
	/* All commands with 0 operands */
	if(command_type == 14 || command_type == 15) {
		if(token_count != (is_symbol + 1)) {
			raise_error_in_line(INVALID_OPERANDS, line_num);
			return;
		}
	}
	
	/* Check if valid amount of commas and that there are no consecutive commas */
	if (!error_check_commas(line, expected_commas)) {
		raise_error_in_line(INVALID_COMMAS, line_num);
	}
}

int error_check_commas(char* line, int expected_commas) {
	int i;
	int commas = 0;
	char* copy = utils_remove_spaces(line); /* Remove spaces & tabs */
	
	/* This is an end case, check the last char of the string seperately to avoid mishandling end-cases*/
	if (copy[strlen(copy) - 1] == ',') {
		free(copy);
		return FALSE;
	}
	
	/* Count each comma and check for consecutive commas in line */
	for(i = 0; i < strlen(copy) - 1; i++) {
		if (copy[i] == ',') {
			if(copy[i+1] ==',') {
				free(copy);
				return FALSE;
			}
			
			commas++;
		}
	}
	
	free(copy);
	return (commas == expected_commas);
}

