#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdlib.h>

/* 
 * This function takes an input string 'input' and appends the provided 'extension' to it.
 * It allocates memory for the new string and returns the pointer to the new string.
 */
char* utils_format_file_name(char*, const char*);

/* 
 * This function duplicates a string to a new allocated block of memory.
 */
char* utils_duplicate_string(char*);


char** utils_tokenize(char*, int*, const char*);

/* 
 * Frees memory for an array of tokens.
 */
void utils_free_tokens(char**, int);

/* 
 * This function merges an array of tokens into a single string, separating each token with spaces.
 * It returns a newly allocated string containing the merged tokens.
 */
char* utils_merge_tokens(char**, int);

/* 
 * This function removes spaces in a line and returns a pointer to a new string.
 */
char* utils_remove_spaces(char*);

#endif