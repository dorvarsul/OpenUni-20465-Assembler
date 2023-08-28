#ifndef READER_H
#define READER_H
#include <stdio.h>

/*
 * Opens a file for reading with the specified file name and extension.
 *
 * This function takes a file name and an extension as parameters and constructs
 * the full file name by appending the extension to the given file name. It then
 * attempts to open the file for reading and returns a file pointer if successful.
 * If the file cannot be opened, an error is raised using the raise_error function.
 */
FILE* reader_open_file(char*, const char*);

/*
* Closes the specified file.
 *
 * This function takes a file pointer as a parameter and checks if it is valid.
 * If the file pointer is not NULL, indicating an open file, the function closes
 * the file. It's important to properly close files after using them to free up
 * system resources and prevent memory leaks.
 */
void reader_close_file(FILE*);

#endif