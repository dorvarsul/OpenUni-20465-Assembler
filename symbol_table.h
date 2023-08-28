#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdio.h>

/*
* This function initializes symbol tables
*/
void symbol_table_init();

/*
* This function frees symbol tables
*/
void symbol_table_free();

/*
* This function initializes a new symbol and appends it to the appropriate table
*/
void symbol_table_append(char*, int, int);

/*
* This function prints the symbol table to stdout
*/
void symbol_table_print();

/*
* This function searches for a symbol in all tables by name, returns the index
* of the symbol in its table if found, and -1 if not
*/
int symbol_table_is_symbol_in(char*);

/*
* This function searches for a symbol specifically in the extern table
*/
int symbol_table_is_extern(char*);

/*
* This function searches for a symbol by name and returns its address
*/
unsigned int symbol_table_get_address(char*);

/*
* This function recieves a symbol name and index and changes its type to entry
*/
void symbol_table_change_to_entry(char*, int);

/*
* This function adds memory offset(100) to all instruction symbols and adds
* memory offset + current ic to all data symbols
*/
void symbol_table_update_addresses(int);

/*
* This function takes in a file pointer and writes all symbols which are entry
* types with its corresponding addresses to the file
*/
int symbol_table_make_ent_file(FILE*);

/*
* This function returns the length of the extern symbol table
*/
int symbol_table_get_extern_length();

#endif