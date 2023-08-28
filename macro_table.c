#include "macro_table.h"
#include "error.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>



typedef struct Macro {
	char* title;
	char* info;
} macro;

typedef struct MacroTable {
	struct Macro** list;
	int current_size;
	int total_size;
} macro_table;

/* Global pointer holding the macro table */
macro_table* table;

/*
	This function initializes the macro table
*/
void macro_table_init() {
    table = (macro_table*)malloc(sizeof(macro_table));
	
	if(table == NULL) {
		raise_error(MEMORY_ERROR);
		exit(FATAL_ERROR);
	}
	
	table->list = (macro**)malloc(sizeof(macro*));
	table->total_size = 1;
	table->current_size = 0;
}

/*
	This function frees the macro table
*/
void macro_table_free() {
	int i;
	
	if(table == NULL) return;
	
	/* For each macro in the table, free all fields and the macro itself */
	for(i=0; i < table->current_size; i++) {
		if(table->list[i] != NULL) {
			free(table->list[i]->title);
			free(table->list[i]->info);
			free(table->list[i]);
		}
	}
	
	/* Free table */
	free(table->list);
	free(table);
}

/*
	This function adds a new macro to the table
*/
void macro_table_add_macro(char* title) {
	/* Initialize new macro */
	macro* mcr = (macro*)malloc(sizeof(macro));
	
	mcr->title = utils_duplicate_string(title);
	mcr->info = (char*)calloc(sizeof(char), 1);
	
	/* Add macro to table */
	if(table->current_size == table->total_size) {
		table->list = (macro**)realloc(table->list, (++(table->total_size)) * sizeof(macro*));
		
		if (table == NULL) {
			raise_error(MEMORY_ERROR);
			exit(FATAL_ERROR);
		}
	}
	
	table->list[table->current_size++] = mcr;
}

/*
	This function appends string to last table's macro
*/
void macro_table_append_to_last_macro(char* info) {
	size_t calculated_length;
	char* copy = NULL;
	macro* mcr;
	
	if (table == NULL || table->list == NULL || table->current_size == 0) return;
	
	/* Pointer to the last macro for convenience */
	mcr = table->list[table->current_size - 1];
	
	/* Calculate length needed to hold both the current info and the new info combined */
	calculated_length = strlen(mcr->info) + strlen(info) + 2;
	
	/* Allocate memory */
	copy = (char*)malloc(calculated_length * sizeof(char));
	
	/* Copy current info and append new info */
	strcpy(copy, mcr->info);
	strcat(copy, info);
	strcat(copy, "\n");
	
	/* Free every byte of memory needed for this operation */
	free(mcr->info);
	mcr->info = copy;
}

/*
	This function checks if a certain macro is in the table by name
	Returns the index to the macro, and -1 if the macro is not in the table
*/
int macro_table_is_macro_in(char* title) {
	int i;
	
	for(i=0; i < table->current_size; i++) {
		if(!strcmp(title, table->list[i]->title)) {
			return i;
		}
	}
    return -1;
}

/*
This function returns the info a macro
*/
char* macro_table_get_mcr_info(int index) {
	return (table->list[index]->info);
}