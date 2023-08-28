#include "translator.h"
#include "constants.h"
#include "symbol_table.h"
#include "writer.h"
#include "error.h"
#include "globals.h"
#include <string.h>
#include <stdlib.h>

const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
* This function converts an address value to its binary representation by extracting
* each bit of the address using a mask and stores the resulting binary string in
* the 'binary_coding' array.
*/
static void convert_address_to_binary(char*, unsigned int);

/*
 * This function converts a number to its binary representation by extracting each bit
 * of the number using a mask and stores the resulting binary string in the
 * 'binary_coding' array.
 */
static void convert_number_to_binary(char*, int);

/*
Translates a word of binary coding into a base64 format and writes it to the output file.
 */
void translator_translate_word(FILE* ob_file, char* coding) {
    unsigned int value = 0;
    int i;
    
    for(i=0; i < WORD_SIZE; i++) {
        value <<= 1;
        value |= (coding[i] - '0');
    }
    
    fprintf(ob_file, "%c%c\n", base64[(value>>6) & 0x3F], base64[value & 0x3F]);
}

/*
Encode an instruction operand into its binary representation.
*/
void translator_encode_operand(char* coding, char* token, int type, FILE* ext_file, int operand_index) {
	if(type == LABEL) {
		/* If operand is a label and extern */
		if(symbol_table_is_extern(token)) {
			strcpy(coding, "0000000000");
			strcat(coding, "01");
			
			/* Add line to EXT file since we used and extern label in some instruction */
			writer_add_ext_to_file(ext_file, token, ic + operand_index);
		}else{
			/* If is not extern encode symbol address */
			convert_address_to_binary(coding, symbol_table_get_address(token));
			strcat(coding, "10");
		}
	}
	
	if(type == ABSOLUTE) {
		/* Convert absolute value to binary */
		convert_number_to_binary(coding, (atoi(token)));
		strcat(coding, "00");
	}
}


/*
Encodes a data operand into its binary representation.
*/
void translator_encode_data(char* coding, int num) {
    int i;
	int mask = 1 << 11;
    
	/* If number cannot be represented by 12 bits raise errors */
    if (num < MIN_DATA_OPERAND || num > MAX_DATA_OPERAND) {
        raise_error_in_line(OVERFLOW_DATA_OPERAND, line_num);
    }
    
	/* Assign each bit to its corressponding place */
    for(i=0; i < WORD_SIZE; i++) {
		coding[i] = (mask & num)? '1':'0';
		mask>>=1;
	}
	
	/* End with null character */
	coding[WORD_SIZE] = '\0';
}

/*
This function encodes a char operand to its binary representation
*/
void translator_encode_char(char* coding, char character) {
	int i;
	int mask = 1 << 11;
	
	for(i=0; i < WORD_SIZE; i++) {
		/* Check if the current bit is set and encode it as '1' or '0' */
		coding[i] = (character & mask)? '1':'0';
		/* Shift the mask to the right to check the next bit */
		mask >>= 1;
	}
	/* Add null character */
	coding[WORD_SIZE] = '\0';
}

/*
This function converts an address value to its binary representation by extracting
each bit of the address using a mask and stores the resulting binary string in
the 'binary_coding' array.
 */
static void convert_address_to_binary(char* binary_coding, unsigned int address) {
	int i;
	int mask = 1 << 9;
	
	for(i=0; i < WORD_SIZE - 2; i++) {
		binary_coding[i] = (address & mask)? '1':'0';
		mask >>= 1;
	}
	
	binary_coding[WORD_SIZE] = '\0';
}

/*
 This function converts a number to its binary representation by extracting each bit
 of the number using a mask and stores the resulting binary string in the
 'binary_coding' array.
*/
static void convert_number_to_binary(char* binary_coding, int number) {
	int mask = 1 << 9;
	int i;
	
	for(i=0; i < WORD_SIZE - 2; i++) {
		/* Check if the current bit is set and encode it as '1' or '0' */
		binary_coding[i] = (number & mask)? '1':'0'; 
		/* Shift the mask to the right to check the next bit */
		mask >>= 1;
	}
	
	/* Add null character */
	binary_coding[WORD_SIZE] = '\0';
}
