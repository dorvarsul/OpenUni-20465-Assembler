#include "utils.h"
#include "error.h"
#include "constants.h"

/* 
This function takes an input string 'input' and appends the provided 'extension' to it.
It allocates memory for the new string and returns the pointer to the new string.
*/
char* utils_format_file_name(char* input, const char* extension){
	size_t inputLen = strlen(input);
    size_t extensionLen = strlen(extension);

    /* Allocate memory for the new string, including space for the extension and the null terminator */
    char* result = (char*)malloc(inputLen + extensionLen + 1);
	result[inputLen + extensionLen] = '\0'; /* Null-terminate the string */

	if(result == NULL){
		raise_error(MEMORY_ERROR);
		exit(FATAL_ERROR);
	}
	
	strcpy(result, input); /* Copy the input string to the new memory location */
	strcat(result, extension); /* Concatenate the extension to the new string */
	
	return result;
}

/*
This function duplicates a string to a new allocated block of memory
*/
char* utils_duplicate_string(char* source) {
    size_t length = strlen(source);

    /* Allocate memory for the new string, including space for the null terminator */
    char* dest = (char*)malloc(length + 1);

    if (dest != NULL) {
        /* Copy the content of the 'source' string to the new memory location */
        strcpy(dest, source);
    }

    return dest;
}

char** utils_tokenize(char* input_string, int* tokens_count, const char* delim){
	char* token; /* Current token */
	char* copy_string; /* Copy to input to avoid manipulating original */
	char** tokens; /* Tokens array */
	int count = 0; /* Number of tokens inputted so far */
	
	/* Allocate memory and handle errors */
	tokens = (char**)malloc(sizeof(char*) * MAX_TOKENS);
	
	if(tokens == NULL){
		raise_error(MEMORY_ERROR);
		exit(FATAL_ERROR);
	}
	
	/* Copy original string to copy and check for errors */
	copy_string = utils_duplicate_string(input_string);
	
	if(copy_string == NULL){
		raise_error(MEMORY_ERROR);
		free(tokens);
		exit(FATAL_ERROR);
	}
	
	/* Start dividing into tokens */
	token = strtok(copy_string, delim);
	
	while(token != NULL && count < MAX_TOKENS){
		tokens[count] = utils_duplicate_string(token);
		
		if(tokens[count] == NULL){
			raise_error(MEMORY_ERROR);
			exit(FATAL_ERROR);
		}
		count++;
		token = strtok(NULL, delim);
	}
	
	free(copy_string);
	*tokens_count = count;
	return tokens;
}

/* Frees tokens */
void utils_free_tokens(char** tokens, int token_count){
	int i;
	if(tokens == NULL) return;
	
	for(i=0; i < token_count; i++){
		if(tokens[i] != NULL){
			free(tokens[i]);
		}
	}
	
	free(tokens);
}

char* utils_merge_tokens(char** tokens, int token_count) {
    int i, len = 0;
    char* result_string;

    /* Calculate the total length of the merged string including spaces and newline */
    for (i = 0; i < token_count; i++) {
        len += strlen(tokens[i]) + 1;
    }
    
    /* Add space for spaces between tokens and the newline */
    len += token_count; /* token_count + spaces */

    /* Allocate memory for the merged string */
    result_string = (char*)malloc(len * sizeof(char));

    /* Check if memory allocation was successful */
    if (result_string == NULL) {
        raise_error(MEMORY_ERROR);
        exit(FATAL_ERROR);
    }

    /* Copy the first token to the merged string */
    strcpy(result_string, tokens[0]);

    /* Add a space and the next token for each subsequent token */
    for (i = 1; i < token_count; i++) {
        strcat(result_string, " ");
        strcat(result_string, tokens[i]);
    }

    return result_string;
}

/*
This function removes spaces in a line and returns pointer to new string
*/
char* utils_remove_spaces(char* line) {
	int size;
	int i, j = 0;
	char* result;
	
	size = strlen(line);
	result = (char*)malloc(size * sizeof(char));
	
	for(i=0; i < size; i++) {
		if (line[i] != ' ' && line[i] != '\n' && line[i] != '\t') {
			result[j++] = line[i];
		}
	}
	result[j] = '\0';
	
	return result;
}