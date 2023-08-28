#ifndef IMAGE_H
#define IMAGE_H
#include <stdio.h>

/*
* Initializes the image data structure for instructions and data
*/
void image_init();

/*
* Frees the memory allocated for the image data structure and its contents
*/
void image_free();

/*
* Prints the contents of the image, including instructions and data
*/
void image_print();

/*
* Appends a list of binary instructions to image's instruction array
*/
void image_append_to_instructions(char**, int);

/*
* Appends a binary encoding of data to image's data array
*/
void image_append_to_data(char**, int);

/*
* Translates image's instructions and data into base 64 format and writes it to file
*/
void image_translate(FILE*);

#endif