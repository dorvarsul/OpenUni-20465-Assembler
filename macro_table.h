#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

/*
* This function initializes the macro table
*/
void macro_table_init();

/*
* This function frees the macro table
*/
void macro_table_free();

/*
* This function adds a new macro to the table 
*/
void macro_table_add_macro(char*);

/*
* This function appends string to last table's macro
*/
void macro_table_append_to_last_macro(char*);

/*
* This function checks if a certain macro is in the table by name
* Returns the index to the macro, and -1 if the macro is not in the table
*/
int macro_table_is_macro_in(char*);

/*
* This function recieves an index and returns the macro's info at the index
*/
char* macro_table_get_mcr_info(int);

#endif