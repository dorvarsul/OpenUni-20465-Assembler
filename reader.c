#include "reader.h"
#include "utils.h"
#include "error.h"

/* 
Open a file with the specified file name and extension for reading.
*/
FILE* reader_open_file(char* file_name, const char* extension){
    /* Add extension to file name */
    char* full_file_name = utils_format_file_name(file_name, extension);
    FILE* reader_file;
    
    printf("----------\nCurrent file: %s\n----------\n", full_file_name);
    printf("Opening...\n");

    reader_file = fopen(full_file_name, "r"); /* Open file */
    
    /* Check if exists */
    if(!reader_file){
        raise_error(CANT_READ_FILE);
    }
    else{
        printf("Success!\n");
    }
    
    /* Free name and returns file pointer */
    free(full_file_name);
    return reader_file;
}

/*
Close the specified file.
*/
void reader_close_file(FILE* reader_file){
    if(reader_file != NULL){
        fclose(reader_file);
    }
}
