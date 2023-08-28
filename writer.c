#include "writer.h"
#include "utils.h"
#include "error.h"
#include "symbol_table.h"
#include "globals.h"
#include "constants.h"
#include "image.h"

/* 
 Open a file for writing with the given file_name and extension
 */
FILE* writer_open_file(char* file_name, const char* extension){
    FILE* file;
    /* Create the full_file_name by concatenating file_name and extension */
    char* full_file_name = utils_format_file_name(file_name, extension);

    /* Open the file for writing mode */
    file = fopen(full_file_name, "w");

    /* Check if file opening was successful */
    if(file == NULL) {
        raise_error(MEMORY_ERROR);
        exit(FATAL_ERROR);
    }

    /* Release memory used for full_file_name, as it is no longer needed */
    free(full_file_name);
    return file;
}

/*
 Write an array of tokens to the given file, separated by spaces, followed by a newline.
 */
void writer_write_tokens_to_file(FILE* file, char** tokens, int token_count) {
    int i;
    
    /* Check if the file, tokens array, or token_count is invalid, then return immediately */
    if(file == NULL || tokens == NULL || token_count == 0) return;
    
    /* Loop through the tokens array and write each token followed by a space to the file */
    for(i = 0; i < token_count; i++) {
        fprintf(file, "%s ", tokens[i]);
    }
    fprintf(file, "\n");
}

/*
 Write a single string to the given file, followed by a newline.
*/
void writer_write_string_to_file(FILE* file, char* line) {
    /* Check if the file or line is invalid, then return immediately */
    if(file == NULL || line == NULL) return;
    
    /* Write the line followed by a newline to the file */
    fprintf(file, "%s", line);
}

/*
 Remove a file with the given file_name and extension.
*/
void writer_remove_file(char* file_name, const char* extension){
    /* Create the full_file_name by concatenating file_name and extension */
    char* full_file_name = utils_format_file_name(file_name, extension);

    /* Remove the file using the remove function from stdio.h */
    remove(full_file_name);

    /* Release memory used for full_file_name, as it is no longer needed */
    free(full_file_name);
}

/*
This function recieves a file pointer, an external name and an external address
and writes the name and address to the file
*/
void writer_add_ext_to_file(FILE* file, char* ext_name, int ext_address) {
    if(file == NULL) return;
    
    fprintf(file, "%s\t%d\n", ext_name, ext_address);
}

/*
This function recieves a file name, creates the output files by the file name with corresponding
extension, writes a message to stdout and translates the entire image into this output files
*/
void writer_write_output_files(char* file_name) {
    int counter;
    
    /* Create .ob and .ent files */
    FILE* ob_file = writer_open_file(file_name, ".ob");
    FILE* ent_file = writer_open_file(file_name, ".ent");
    
    printf("Translating files...\n");
    
    fprintf(ob_file, "%d %d\n", ic - MEMORY_OFFSET, dc);
    
    /* Translate image to ob file */
    image_translate(ob_file);
    
    /* Write all entries symbols to ent file */
    counter = symbol_table_make_ent_file(ent_file);
    
    /* If no entries written than remove .ent file */
    if(counter == 0) {
        printf("No entries found!\nRemoving ENT file...\n");
        writer_remove_file(file_name, ".ent");
    }
    
    /* Close files */
    fclose(ob_file);
    fclose(ent_file);
}
