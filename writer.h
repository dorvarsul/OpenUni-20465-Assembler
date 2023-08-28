#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>

/*
* Open a file for writing with given file_name and extension
*/
FILE* writer_open_file(char*, const char*);

/*
* This function writes an array of tokens to given file, seperated by spaces
* and followed by a new line
*/
void writer_write_tokens_to_file(FILE*, char**, int);

/*
* This function writes a string to a given file followed by a new line
*/
void writer_write_string_to_file(FILE*, char*);

/*
* Recieves a file name and an extension and removes the file
*/
void writer_remove_file(char*, const char* extension);

/*
* This function recieves a filepointer, external name, address and writes it
* to given file
*/
void writer_add_ext_to_file(FILE*, char*, int);

/*
* This function recieves a filename, creates the output files and translates
* the entire image into output files
*/
void writer_write_output_files();

#endif