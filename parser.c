#include "parser.h"
#include "utils.h"
#include "error.h"
#include "writer.h"
#include "reader.h"
#include "globals.h"
#include "constants.h"
#include "symbol_table.h"
#include "macro_table.h"
#include "translator.h"
#include "lexer.h"

/*---------------------------------------------------------- 
Pre-Assembly phase of the parser
----------------------------------------------------------*/

/* Check if the line contains no tokens (empty line). */
static int is_empty_line(char**, int);

/* Check if the line is a comment (starts with a semicolon ';'). */
static int is_comment(char**, int);

/* Handle the procedure when inside a macro. */
static int in_macro_procedure(char**, int);

/* Handle the procedure when not inside a macro. */
static int not_in_macro_procedure(char**, int, FILE*);


/* Assemble the input file and generate the .am file. 
    Return 1 if successful, and 0 otherwise */
int parser_assemble_file(FILE* file, char* file_name){
	/* Assuming input is less than 80 characters */
    char input_line[MAX_LINE_LENGTH]; /* Current line being processed */
    FILE* writer_file;
    
    int token_count = 0; /* Number of tokens in a line */
    char** tokens; /* Array of tokens */
    
    int in_macro = 0; /* Flag indicating if currently inside a macro */
    /* macro* current_macro; Pointer to the current macro */
    
    
    /* table = list_init_macro_table(); Initialize the macro table */
    macro_table_init();
    writer_file = writer_open_file(file_name, ".am"); /* Open .am file for writing */
	line_num = 0; /* Initialize current line num to 1 */
    errors = 0; /* Initialize error count to zero */
    
    while(fgets(input_line, MAX_LINE_LENGTH, file) != NULL){
        /* Tokenize string into an array of tokens */
        tokens = utils_tokenize(input_line, &token_count, " \t\n\r");
		line_num++;
        
		
        if(is_empty_line(tokens, token_count))
            continue;
        
        if(is_comment(tokens, token_count))
            continue;
        
        /* Check if currently inside a macro or not, initiate proper procedure, and update the 'in_macro' flag */
        if(in_macro)
            in_macro = in_macro_procedure(tokens, token_count);
        else
            in_macro = not_in_macro_procedure(tokens, token_count, writer_file);
        
        utils_free_tokens(tokens, token_count); /* Free memory used by tokens */
    }
    
    /* list_free_macro_table(table);  Free memory used by the macro table */
    macro_table_free();
    printf("End of file\n");
    
    /* If there are any errors, remove the .am file */
    if(errors) {
        writer_remove_file(file_name, ".am");
    }
    fclose(writer_file); /* Close the .am file */
    
    /* Return TRUE if there are no errors, FALSE otherwise */
    return (errors == 0) ? TRUE : FALSE;
}




/* Handle the procedure when inside a macro.
 * Returns:
 *  - TRUE if the procedure continues inside a macro, FALSE if 'endmcro' is found.
 */
static int in_macro_procedure(char** tokens, int token_count){
    char* line;
    
    /* Assuming 'endmcro' declaration is valid, and is the only token in the input line */
    /* If we reach 'endmcro', return FALSE to indicate the end of the macro */
    if(!strncmp(tokens[0], "endmcro", 7)){
        /* If there are tokens other than 'endmcro', display an error */
        if(token_count != 1){
            raise_error_in_line(INVALID_ENDMCRO, line_num);
        }
        return FALSE;
    }
    
    line = utils_merge_tokens(tokens, token_count); /* Merge tokens into a single line */
    /* list_macro_append(mcr, line);  Append the line to the current macro's info */
    macro_table_append_to_last_macro(line);
    
    free(line); /* Free the memory used by the merged line */
    return TRUE; /* Continue in the macro procedure */
}


/* Handle the procedure when not inside a macro.
 * Returns:
 *  - TRUE if the procedure continues not inside a macro, FALSE if a 'mcro' is found.
 */
static int not_in_macro_procedure(char** tokens, int token_count, FILE* writer_file){
    int index_to_macro;
    
    /* If 'mcro' declaration is found */
    if(!strncmp(tokens[0], "mcro", 4)){
        /* If there are not exactly 2 tokens in line(mcro and title), display an error */
        if (token_count != 2 || error_invalid_keyword(tokens[1])) {
            raise_error_in_line(INVALID_MCRO, line_num);
        }
        
        macro_table_add_macro(tokens[1]);
        return TRUE;
    }
    
	/* Check if current token exists as a macro in the table */
    /* current_macro = list_find_macro(tokens[0]); */
    index_to_macro = macro_table_is_macro_in(tokens[0]);
	
    if (index_to_macro != -1) {
        writer_write_string_to_file(writer_file, macro_table_get_mcr_info(index_to_macro)); /* Write macro's info to file */
        return FALSE; /* Continue not in the macro procedure */
    }
    
    writer_write_tokens_to_file(writer_file, tokens, token_count); /* Write tokens to file */
    return FALSE; /* Continue not in the macro procedure */
}





/* Check if the line contains no tokens (empty line).
 * Returns:
 *  - TRUE if the line is empty, FALSE otherwise.
 */
static int is_empty_line(char** tokens, int token_count){
    if(token_count == 0){
        utils_free_tokens(tokens, token_count); /* Free memory used by tokens */
        return TRUE;
    }
    return FALSE;
}

/* Check if the line is a comment (starts with a semicolon ';').
 * Returns:
 *  - TRUE if the line is a comment, FALSE otherwise.
 */
static int is_comment(char** tokens, int token_count){
    if(!strncmp(tokens[0], ";", 1)){
        utils_free_tokens(tokens, token_count); /* Free memory used by tokens */
        return TRUE;
    }
    return FALSE;
}

















/*---------------------------------------------------------- 
First pass phase of the assembler
----------------------------------------------------------*/

/* Check if the first token is a valid symbol (label). */
static int is_first_token_symbol(char**, int);

/* Check if the token indicates a data declaration. */
static int valid_symbol(char*);

/* Check if the token indicates a string declaration. */
static int is_data(char*);

/* Calculate the number of characters needed for a string. */
static int is_string(char*);

/* Calculate the number of integers in a .data declaration. */
static int calculate_chars(char**, int, int, char*);

/* Check if a symbol exists in the symbol table. */
static int calculate_integers(char**, int, int);

/* Calculate the length (number of rows) required for the instruction's encoding. */
static int is_symbol_in_table(char**, int);

/* Check if a token is a valid register. */
static int calculate_ic_length(char**, int, int);

/* Check if a line indicates an .extern declaration. */
static int is_register(char*);

/* Check if a line indicates an .entry declaration. */
static int is_extern(char**, int);

/* Add extern symbols to the symbol table. */
static int is_entry(char**, int);

/* Perform the first pass of the assembly process. */
static void add_externs_to_table(char**, int, int);


/* 
This first pass is responsible for initial error checking in the file, and the creation
of a symbol table. and classification of each symbol
*/
void parser_first_pass(FILE* file) {
	
	char input_line[MAX_LINE_LENGTH] = {0}; /* Input line */
	char** tokens = NULL; /* Array of tokens */
	int token_count = 0; /* Num of Tokens */
	
	int is_symbol; /* Is Symbol / Label  flag*/
    
	ic = 0; /* Initialize ic to 0 */
    dc = 0; /* Initialize dc to 0 */
	line_num = 0; /* Initialize number of line to 0 */
	errors = 0; /* Initialize errors to 0 */
    
    fprintf(stdout, "Starting initial error handling...\nBuilding symbol table ...\n");
	
	while(fgets(input_line, MAX_LINE_LENGTH, file) != NULL) {
        line_num++;
        
        /* Tokenize line to different tokens */
        tokens = utils_tokenize(input_line, &token_count, " ,\t\n\r");
        
        /* Check if first token is symbol then turn flag to TRUE */
        is_symbol = is_first_token_symbol(tokens, token_count);
        
        /* If there is a symbol declaration and it already exists, raise error and continue to next line */
        if (is_symbol) {
            if (is_symbol_in_table(tokens, 0) || !valid_symbol(tokens[0])) {
                utils_free_tokens(tokens, token_count);
                continue;
            }
            
            /* If its an empty symbol declaration raise error */
            if(token_count == 1) {
                raise_error_in_line(EMPTY_LABEL, line_num);
                utils_free_tokens(tokens, token_count);
                continue;
            }
        }
        
        /* If its a .data declaration */
        if(is_data(tokens[is_symbol])) {
            /* If its also a symbol add it to symbol table */
            if(is_symbol) {
                symbol_table_append(tokens[0], DC_TYPE, dc);
            }
            
            /* Calculate number of rows needed for the encoding of the data */
            dc += calculate_integers(tokens, token_count, is_symbol + 1);
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If its a .string declaration */
        if(is_string(tokens[is_symbol])) {
            /* If its also a symbol, add to symbol table */
            if(is_symbol){
                symbol_table_append(tokens[0], DC_TYPE, dc);
            }
            
            /* Calculate number of rows needed for the enoding of the string */
            dc += calculate_chars(tokens, token_count, is_symbol + 1, input_line);
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If its extern we handle it in a seperate function */
        if(is_extern(tokens, is_symbol)) {
            add_externs_to_table(tokens, token_count, is_symbol);
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If its .entry declaration we continue to next line since we only handle it in the second pass */
        if(is_entry(tokens, is_symbol)) {
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If non of the ifs so far are met, that means the line is an instruction */
        
        /* Check for initial errors in the instruction line */
        error_check_instruction(input_line, is_symbol, tokens, token_count);
        
        /* If its symbol than add it to the symbol table as instruction */
        if(is_symbol) {
            symbol_table_append(tokens[0], IC_TYPE, ic);
        }
    
        /* Increment IC with the total lines required by the instruction */
        ic += calculate_ic_length(tokens, token_count, is_symbol);
        utils_free_tokens(tokens, token_count);
	}
    
    /* Update symbol addresses (since memory begins at 100 and data begins at 100 + IC) */
    symbol_table_update_addresses(ic);
    
    /* Debugging purposes */
    fprintf(stdout, "Symbol Table:\n");
    symbol_table_print();
    fprintf(stdout, "\n\n");
}

/* Validate if a symbol (label) is valid. */
static int valid_symbol(char* name) {
    size_t length;
    char* copy;
    
    copy = utils_duplicate_string(name);
    length = strlen(name);
    
    /* Remove trailing ':' if present */
    if(copy[length-1] == ':') {
        copy[length-1] = '\0';
    }
    
    /* If symbol is too long or doesnt start with alphabetic letter or is a reserved 
    keyword, than raise error. */
    if (length >= MAX_LABEL_LENGTH || !isalpha(name[0]) || error_invalid_keyword(copy)) {
        raise_error_in_line(INVALID_LABEL, line_num);
        free(copy);
        return FALSE;
    }
    
    free(copy);
    return TRUE;
}

/* Check if the first token in a line is a symbol (label). */
static int is_first_token_symbol(char** tokens, int token_count) {
    int length = strlen(tokens[0]);
    
    /* If last char of the string is ':' than return true */
    if (tokens[0][length-1] == ':') {
        return TRUE;
    }
    return FALSE;
}

/* Check if a token represents a .data declaration. */
static int is_data(char* token) {
    return !strcmp(token, ".data");
}

/* Check if a token represents a .string declaration. */
static int is_string(char* token) {
    return !strcmp(token, ".string");
}

/* Calculate the number of characters needed for a string. */
static int calculate_chars(char** tokens, int token_count, int start_index, char* line) {
    int i;
    int counter = 0; /* Counts the number of characters between double quotes */
    int quotes = 0; /* Number of double quotes encountered */
    
    /* Continue this function only if there is 1 argument to the .string
        If it is too little we raise error */
    if (token_count == start_index) {
        raise_error_in_line(INVALID_STRING, line_num);
        return FALSE;
    }
    
    /* This loop counts the number of character between the first double quotes and the second
    double quotes, if it encounters a third one than we raise error */
    for(i=0; i < strlen(line); i++) {
        if (quotes == 0) {
            if(line[i] == '\"') {
                quotes++;
                continue;
            }
        }else if(quotes == 1){
            if(line[i] == '\"') {
                quotes++;
            }else{
                counter++;
            }
        }else if(line[i] == '\"') {
            /* If we encounter a thid double quotes we raise invalid string error */
            raise_error_in_line(INVALID_QUOTES, line_num);
            break;
        }
    }
    
    /* Return counter + space for null character */
    return ++counter;
}

/* Calculate the number of integers in a .data declaration. */
static int calculate_integers(char** tokens, int token_count, int start_index) {
    int i;
    int counter = 0;
    
    for(i=start_index; i < token_count; i++) {
        /* If a valid integer increment counter */
        if (atoi(tokens[i]) || !strcmp(tokens[i], "0")) {
            counter++;
        }
    }
    
    /* If no valid integers met, raise error */
    if(counter == 0) {
        raise_error_in_line(INVALID_DATA, line_num);
    }
    
    return counter;
}

/* Check if a symbol (label) is already in the symbol table. */
static int is_symbol_in_table(char** tokens, int index) {
    char* copy;
    int length;
    int result;
    
    copy = utils_duplicate_string(tokens[index]);
    length = strlen(copy);
    
    /* If the symbol ends with ':' we remove it */
    if(copy[length - 1] == ':') {
        copy[length - 1] = '\0';
    }
    
    result = symbol_table_is_symbol_in(copy);
    
    /* Symbol alredy exists */
    if(result != INVALID) {
        raise_error_in_line(SYMBOL_ALREDY_EXISTS, line_num);
    }
    
    free(copy);
    return (result >= 0)? TRUE:FALSE;
}

/* This function calculates the amount of encoded lines required for the instruction */
static int calculate_ic_length(char** tokens, int token_count, int is_symbol) {
    int number_of_lines = 1; /* Atleast 1 line is required for the operation itself */
    
    if (is_symbol) {
        /* If the line is (label:) (operation) (operand) (operand) */
        if (token_count == 4) {
            /* If both operands are register we need 1 extra line */
            if (is_register(tokens[2]) && is_register(tokens[3])) {
                number_of_lines++;
            }else {
                number_of_lines += 2;
            }
        }
        
        /* If the line is: (label:) (operation) (operand) */
        if (token_count == 3) {
            number_of_lines++;
        }
    } else {
        /* If the line is: (operation) (operand) (operand) */ 
        if(token_count == 3) {
            if (is_register(tokens[1]) && is_register(tokens[2])) {
                number_of_lines ++;
            }else {
                number_of_lines += 2;
            }
        }
        
        /* If the line is: (operation) (operand) */
        if (token_count == 2) {
            number_of_lines++;
        }
    }
        
    return number_of_lines;
}

/* Check if a token represents a valid register. */
static int is_register(char* token) {
    const char* registers[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
    int i;
    
    for(i=0; i < 8; i++) {
        if(!strcmp(token, registers[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Check if a line indicates an .extern declaration. */
static int is_extern(char** tokens, int is_symbol) {
    if (!strcmp(tokens[is_symbol], ".extern")) {
        if(is_symbol){
            raise_warning_in_line(UNNECESSARY_SYMBOL, line_num);
        }
        return TRUE;
    }
    return FALSE;
}

/* Check if a line indicates an .entry declaration. */
static int is_entry(char** tokens, int is_symbol) {
    if(!strcmp(tokens[is_symbol], ".entry")) {
        if(is_symbol) {
            raise_warning_in_line(UNNECESSARY_SYMBOL, line_num);
        }
        return TRUE;
    }
    return FALSE;
}

/* Add extern symbols to the symbol table. */
static void add_externs_to_table(char** tokens, int token_count, int is_symbol) {
    int i;
    int length;
    
    for(i=is_symbol + 1; i < token_count; i++) {
        length = strlen(tokens[i]);
        
        /* Check if parameter is alredy an existing symbol and raise error if so */
        if (is_symbol_in_table(tokens, i)) {
            continue;
        }
        
        /* Checks that parameter is a valid symbol name */
        if (length >= MAX_LABEL_LENGTH || !isalpha(tokens[i][0])) {
            raise_error_in_line(INVALID_LABEL, line_num);
            continue;
        }
        
        /* If parameter is valid append to symbol table */
        symbol_table_append(tokens[i], EXTERN_TYPE, 0);
    }
}










/*----------------------------------------------------
Second pass phase of the assembler
----------------------------------------------------*/

/* 
The second pass phase is the phase where we check for more complex errors in the file
and create an initial translation of the lines into binary.
*/
int parser_second_pass(FILE* file, char* file_name) {
    char input_line[MAX_LINE_LENGTH]; /* Current line being parsed */
    int token_count; /* Amount of tokens */
    char** tokens; /* Line divided into tokens */
    int is_symbol; /* Is symbol flag*/
    int i, index; /* Indices */
    FILE* ext_file; /* Pointer to .ext file */
    
    /* If amount of lines required by instructions and data exceeds 924 we raise memory overflow error */
    if (ic + dc > MEMORY_SIZE) {
        raise_error(MEMORY_OVERFLOW);
        return FALSE;
    }
    
    ext_file = writer_open_file(file_name, ".ext"); /* Create new .ext file */
    
    /* Initialize line num and offset ic count */
    line_num = 0;
    ic = MEMORY_OFFSET;
    
    while(fgets(input_line, MAX_LINE_LENGTH, file)) {
        line_num++;
        /* Tokenize line */
        tokens = utils_tokenize(input_line, &token_count, " ,\t\n\r");
        
        /* Check if there's a symbol declaration */
        is_symbol = is_first_token_symbol(tokens, token_count);
        
        /* If .data declaration encode to proper location in memory */
        if(is_data(tokens[is_symbol])) {
            lexer_analyze_data(tokens, token_count, is_symbol);
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If .string declaration encode to proper location */
        if(is_string(tokens[is_symbol])) {
            lexer_analyze_string(input_line);
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If .extern declaration ignore */
        if(is_extern(tokens, is_symbol)) {
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If .entry declaration */
        if(is_entry(tokens, is_symbol)) {
            /* For each parameter in the line we check if parameter is a non-external existing symbol*/
            for(i=is_symbol+1; i < token_count; i++) {
                
                if (symbol_table_is_extern(tokens[i])) {
                    raise_error_in_line(ENTRY_DEFINED_AS_EXTERN, line_num);
                }
                
                index = symbol_table_is_symbol_in(tokens[i]);
                if(index == -1) raise_error_in_line(ENTRY_NOT_FOUND, line_num);
                
                /* Change symbol type to entry */
                symbol_table_change_to_entry(tokens[i], index);
            }
            utils_free_tokens(tokens, token_count);
            continue;
        }
        
        /* If not .data/.string/.extern/.entry that means we encounter instruction line and encode it accordingly*/
        lexer_analyze_operation(tokens, token_count, is_symbol, calculate_ic_length(tokens, token_count, is_symbol), ext_file);
        /* Increment ic by length required */
        ic += calculate_ic_length(tokens, token_count, is_symbol);
        utils_free_tokens(tokens, token_count);
    }
        
    /* If program handled errors or extern table is empty we remove the .ext file */
    reader_close_file(ext_file);
    
    ext_file = reader_open_file(file_name, ".ext");
    if(errors || (fgetc(ext_file)) == EOF) {
        printf("Removing EXT file...\n");
        writer_remove_file(file_name, ".ext");
    }
    
    reader_close_file(ext_file);
    return (errors == 0)? TRUE:FALSE;
}