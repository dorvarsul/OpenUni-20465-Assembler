#include "lexer.h"
#include "symbol_table.h"
#include "globals.h"
#include "error.h"
#include "constants.h"
#include "image.h"
#include "writer.h"
#include "utils.h"
#include "translator.h"
#include <string.h>
#include <stdlib.h>

/* Array of command mnemonics. */
const char* commands[] = {
	"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Array of register mnemonics. */
const char* registers[] = {
	"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"
};

/* Binary encodings for different addressing types. */
const char* addressing_types[] = {
	"000", "001", "000", "011", "000", "101"
};

/* Binary encodings for different commands. */
const char* commands_encoding[] = {
	"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
};

/* Binary encodings for different registers. */
const char* register_encoding[] = {
	"00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111"
};
	
/* This function gets a token and a command_type, and returns the destination type */
static int get_destination_type(char*, int);
/* This functions takes in a token and returns the command type */
static int get_command_type(char*);
/* This function takes in a token and a command type and returns the source type */
static int get_source_type(char*, int);
/* This function takes in a token, returns index to the register array if found, INVALID if not */
static int get_register(char*);


/*
	This function takes in a line from the input file, analyzes its operands and encodes it to binary representation and appends it to image
*/
void lexer_analyze_operation(char** tokens, int token_count, int is_symbol, int amount_of_lines, FILE* ext_file) {
	int command_type;
	int source_type;
	int destination_type;
	/* Allocate enough memory to hold encoded words in a char arrays */
	char** coding = (char**)malloc(sizeof(char*) * amount_of_lines);
	
	/* Get the type of command */
	command_type = get_command_type(tokens[is_symbol]);
	
	/* If command takes 2 operands, classify both destination and source */
	if((command_type >= 0 && command_type <= 3) || command_type == 6) {
		source_type = get_source_type(tokens[is_symbol + 1], command_type);
		destination_type = get_destination_type(tokens[is_symbol + 2], command_type);
	}
	/* If command takes 1 operand, classify operand as destination and assign 0 to source */
	else if(command_type == 4 || command_type == 5 || (command_type >= 7 && command_type <= 13)) {
		source_type = 0;
		destination_type = get_destination_type(tokens[is_symbol + 1], command_type);
	}
	/* If command takes no operands, set source and destination to 0 */
	else {
		source_type = 0;
		destination_type = 0;
	}
	
	/* If we recieve -1 on either the source or destination that means it is an invalid operand to the given command */
	if(source_type == INVALID || destination_type == INVALID) {
		if(source_type == INVALID){
			raise_error_in_line(INVALID_SOURCE_OPERAND, line_num);
		}
		if(destination_type == INVALID) raise_error_in_line(INVALID_DEST_OPERAND, line_num);
		free(coding);
		return;
	}
	
	/* Format binary coding of main command */
	coding[0] = (char*)calloc(sizeof(char), WORD_SIZE + 1); /* Allocate memory for 12 chars & NULL character */
	strcpy(coding[0], addressing_types[source_type]);
	strcat(coding[0], commands_encoding[command_type]);
	strcat(coding[0], addressing_types[destination_type]);
	strcat(coding[0], "00");
	
	/* Format binary coding of operands depending on source and destination */
	if(amount_of_lines == 2) {
		coding[1] = (char*)calloc(sizeof(char), WORD_SIZE + 1);

		/* Both operands are registers*/		
		if(source_type == REGISTER && destination_type == REGISTER) {
			strcpy(coding[1], register_encoding[get_register(tokens[is_symbol + 1])]);
			strcat(coding[1], register_encoding[get_register(tokens[is_symbol + 2])]);
			strcat(coding[1], "00");
		}
		/* Otherwise we are looking at a command with 1 operand so we treat it with destination tpye only */
		else if(destination_type == REGISTER) {
			strcpy(coding[1], "00000");
			strcat(coding[1], register_encoding[get_register(tokens[is_symbol + 1])]);
			strcat(coding[1], "00");
		}
		else{
			/* If not register encode in different function */
			translator_encode_operand(coding[1], tokens[is_symbol+1], destination_type, ext_file, 1);
		}
	}
	
	
	if(amount_of_lines == 3) {
		coding[1] = (char*)calloc(sizeof(char), WORD_SIZE + 1);
		coding[2] = (char*)calloc(sizeof(char), WORD_SIZE + 1);
		
		if(source_type == REGISTER) {
			strcpy(coding[1], register_encoding[get_register(tokens[is_symbol+1])]);
			strcat(coding[1], "00000");
			strcat(coding[1], "00");
		}
		else {
			translator_encode_operand(coding[1], tokens[is_symbol+1], source_type, ext_file, 1);
		}
		
		if(destination_type == REGISTER){
			strcpy(coding[2],"00000");
			strcat(coding[2], register_encoding[get_register(tokens[is_symbol+2])]);
			strcat(coding[2], "00");
		}
		else{
			translator_encode_operand(coding[2], tokens[is_symbol+2], destination_type, ext_file, 2);
		}
	}
	
	image_append_to_instructions(coding, amount_of_lines);
}

/* 
This function takes in an array of token, amount of tokens and a flag if line is symbol
and encodes the data into its binary representation and appends it to the data image
*/
void lexer_analyze_data(char** tokens, int token_count, int is_symbol) {
    int num; /* Converted operand */
    char** coding; /* Array of binary coding */
    int size; /* Amount of encoded binary words */
    int i, index; /* Counters */
    
    size = token_count - is_symbol - 1; /* Num of operands = total tokens - label - .data declaration */
    coding = (char**)malloc(sizeof(char*) * size); /* Allocate memory */
    
    for(i= is_symbol + 1, index = 0; i < token_count; i++, index++) {
		/* Allocate memory for the current binary word */
        coding[index] = (char*)malloc(sizeof(char) * (WORD_SIZE + 1));
		/* Convert binary word to an integer */
        num = atoi(tokens[i]);
		/* Convert integer to an array of chars representing bits */
        translator_encode_data(coding[index], num);
    }
	
	/* Append binary encoding to data image */
	image_append_to_data(coding, size);
}

/*
This function recieves a string, analyzes it and encodes it into
binary representation with the null character
*/
void lexer_analyze_string(char* line) {
    int index, i; /* Counters */
    char** coding; /* Array of binary coding */
	int size = 0; /* Amount of encoded binary words */
	int quotes = 0; /* Amount of quotes detected so far*/
	
	/* We first make sure there is a legitimate .string line by counting number of quotes */
	for(i=0; i < strlen(line); i++) {
		
		if(line[i] == '\"') {
			/* If this is the 3rd double quote we encounter, raise error for invalid string */
			if(quotes == 2) {
				raise_error_in_line(INVALID_QUOTES, line_num);
				return;
			}
			quotes++;
			continue;
		}
		
		/* If 2 > quotes > 0 that means we are inside a string we are supposed to encode */
		if(quotes == 1)
			size++;
	}
	
	size++; /* Add size to encode null-character */
	coding = (char**)malloc(sizeof(char*) * size); /* Allocate memory for each binary word */
	
	/* In this loop we encode every character between the double quotes */
	for(i=0, index=0, quotes=0; i < strlen(line); i++) {
		if(quotes == 2)
			break;
		
		if(line[i] == '"') {
			quotes++;
			continue;
		}
		
		if(quotes == 1) {
			coding[index] = (char*)malloc(sizeof(char) * (WORD_SIZE+1));
			translator_encode_char(coding[index], line[i]);
			index++;
		}
	}
	
	/* Append null character */
	coding[size - 1] = utils_duplicate_string("000000000000");
	image_append_to_data(coding, size);
}

/*
This function gets a token and a command_type, and returns the destination type
*/
static int get_destination_type(char* token, int command_type) {
	if(get_register(token) != -1) {
		return REGISTER;
	}
	
	if(symbol_table_is_symbol_in(token) != -1) {
		return LABEL;
	}
	
	/* Absolute destination type is only valid for command types 1 and 12 */
	if(command_type == 1 || command_type == 12) {
		if(atoi(token) != 0 || !strcmp(token, "0")) {
			return ABSOLUTE;
		}
	}

	return INVALID;
}

/*
This functions takes in a token and returns the command type
*/
static int get_command_type(char* token) {
	int i;
	
	for(i=0; i < AMOUNT_OF_COMMANDS; i++) {
		if(!strcmp(token, commands[i])) {
			return i;
		}
	}
	return INVALID;
}

/*
This function takes in a token and a command type and returns the source type
*/
static int get_source_type(char* token, int command_type) {
	int num;
	
	/* For command type 6 only label is valid for source type*/
	if(command_type == 6) {
		if(symbol_table_is_symbol_in(token)) {
			return LABEL;
		}else{
			raise_error_in_line(INVALID_SOURCE_OPERAND, line_num);
			return INVALID;
		}
	}
	
	
	if(get_register(token) != -1){
			return REGISTER;
	}
	
		
	if(symbol_table_is_symbol_in(token) != -1) {
		return LABEL;
	}
		
	num = atoi(token);
	if(num || !strcmp(token, "0")) {
		/* If absolute value cannot be represented by 10 bits raise error */
		if (num > MAX_NUM_OPERAND || num < MIN_NUM_OPERAND) {
			return INVALID;
		} else {
			return ABSOLUTE;
		}
	}
	return INVALID;
}

/*
This function takes in a token, returns index to the register array if found, INVALID if not
*/
static int get_register(char* token){
	int i;
	
	for(i=0; i < AMOUNT_OF_REGISTERS; i++) {
		if(!strcmp(token, registers[i])) {
			return i;
		}
	}
	return INVALID;
}
