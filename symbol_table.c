#include "symbol_table.h"
#include "utils.h"
#include "constants.h"
#include "error.h"
#include <stdlib.h>


/* Represents a symbol in the assembler */
typedef struct Symbol {
	char* name; /* Symbol name */
	int type; /* Symbol type */
	int address; /* Symbol address */
} symbol;

/* Represents a symbol table in the assembler */
typedef struct SymbolTable {
	struct Symbol** list; /* Pointer to symbol list */
	int current_size; /* Amount of symbols appended so far */
	int total_size; /* Total size allocated for symbol table */
} symbol_table;



/* This functions prints / frees / appends symbol to the specified symbol table */
static void print_table(symbol_table*, int);
static void free_table(symbol_table*);
static void append_to(symbol_table*, symbol*);




/* Global variables to symbol tables */
symbol_table* data_table;
symbol_table* instructions_table;
symbol_table* extern_table;




/* 
	Initialize new symbol table
*/
void symbol_table_init() {
	/* Allocate memory for all tables */
	data_table = (symbol_table*)malloc(sizeof(symbol_table));
	instructions_table = (symbol_table*)malloc(sizeof(symbol_table));
	extern_table = (symbol_table*)malloc(sizeof(symbol_table));
	
	/* Initialize data table */
	data_table->list = (symbol**)malloc(sizeof(symbol*));
	data_table->current_size = 0;
	data_table->total_size = 1;
	
	/* Initialize instructions table */
	instructions_table->list = (symbol**)malloc(sizeof(symbol*));
	instructions_table->current_size = 0;
	instructions_table->total_size = 1;
	
	/* Initialize external table */
	extern_table->list = (symbol**)malloc(sizeof(symbol*));
	extern_table->current_size = 0;
	extern_table->total_size = 1;
}

/*
	Free all symbol tables
*/
void symbol_table_free() {
	if(data_table != NULL) {
		free_table(data_table);		
	}
	
	if(instructions_table != NULL) {
		free_table(instructions_table);
	}
	
	if(extern_table != NULL) {
		free_table(extern_table);
	}
}


/*
	This function initializes a new symbol and appends it to the appropriate table 
*/
void symbol_table_append(char* symbol_name, int symbol_type, int symbol_address) {
	int length;
	
	symbol* sym = (symbol*)malloc(sizeof(symbol)); /* Allocate memory for symbol */
	sym->name = utils_duplicate_string(symbol_name); /* Initialize symbol's name */
	
	
	/* Get index of last non-null character to check if ':' */
	length = strlen(sym->name) - 1;
	
	if(sym->name[length] == ':')
		sym->name[length] = '\0';
	
	/* Assign symbol type and address */
	sym->type = symbol_type;
	sym->address = symbol_address;

	/* According to symbol type append to appropriate table */
	if(symbol_type == DC_TYPE) append_to(data_table, sym);
	if(symbol_type == IC_TYPE) append_to(instructions_table, sym);
	if(symbol_type == EXTERN_TYPE) append_to(extern_table, sym);
}

/*
	This function prints the symbol table for debug purposes
*/
void symbol_table_print() {	
	if(instructions_table != NULL) {
		print_table(instructions_table, IC_TYPE);
	}
	
	if(data_table != NULL) {
		print_table(data_table, DC_TYPE);
	}
	
	if(extern_table != NULL) {
		print_table(extern_table, EXTERN_TYPE);
	}
}


/*
This function searches for a symbol in all three tables according to name, returns the index
of the symbol in its table if found, and -1 if not.
*/
int symbol_table_is_symbol_in(char* name) {
	int i;
	
	for(i=0; i < instructions_table->current_size; i++) {
		if(!strcmp(name, instructions_table->list[i]->name)) {
			return i;
		}
	}
	
	for(i=0; i < data_table->current_size; i++) {
		if(!strcmp(name, data_table->list[i]->name)) {
			return i;
		}
	}
	
	for(i=0; i < extern_table->current_size; i++) {
		if(!strcmp(name, extern_table->list[i]->name)) {
			return i;
		}
	}
	
	return INVALID;
}

/* 
This function searches for a symbol specifically in the extern table
*/
int symbol_table_is_extern(char* name) {
	int i;
	
	for(i=0; i < extern_table->current_size; i++) {
		if(!strcmp(name, extern_table->list[i]->name)) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
This function searches for a symbol and returns its address 
*/
unsigned int symbol_table_get_address(char* name) {
	int i;
	
	for(i=0; i < instructions_table->current_size; i++) {
		if(!strcmp(name, instructions_table->list[i]->name)){
			return instructions_table->list[i]->address;
		}
	}
	
	for(i=0; i < data_table->current_size; i++){
		if(!strcmp(name, data_table->list[i]->name)) {
			return data_table->list[i]->address;
		}
	}
	
	return FALSE;
}

/*
This function recieves a symbol name and index and changes its type to entry
*/
void symbol_table_change_to_entry(char* name, int index) {
	
	if(index == INVALID) {
		return;
	}
	
	/* If index is within table's range and the name fits change to entry*/
	if(index < data_table->current_size && !strcmp(name, data_table->list[index]->name))
		data_table->list[index]->type = ENTRY_TYPE;
	if (index < instructions_table->current_size && !strcmp(name, instructions_table->list[index]->name))
		instructions_table->list[index]->type = ENTRY_TYPE;
}

/*
This function adds MEMORY OFFSET(100) to all instruction symbols and adds
MEMORY OFFSET + current IC to all data symbols
*/
void symbol_table_update_addresses(int ic) {
	int i;
	
	for(i=0; i < instructions_table->current_size; i++) {
		instructions_table->list[i]->address += MEMORY_OFFSET;
	}
	
	for(i=0; i < data_table->current_size; i++) {
		data_table->list[i]->address += MEMORY_OFFSET + ic;
	}
}

/*
This function takes in a file pointer and writes all symbols which are entry types
with its corresponding addresses to the file 
*/
int symbol_table_make_ent_file(FILE* ent_file) {
	int i;
	int counter = 0;
	
	for(i=0; i < instructions_table -> current_size; i++) {
		if(instructions_table->list[i]->type == ENTRY_TYPE) {
			fprintf(ent_file, "%s\t%d\n", instructions_table->list[i]->name, instructions_table->list[i]->address);
			counter++;
		}
	}
	
	for(i=0; i < data_table -> current_size; i++) {
		if(data_table->list[i]->type == ENTRY_TYPE) {
			fprintf(ent_file, "%s\t%d\n", data_table->list[i]->name, data_table->list[i]->address);
			counter++;
		}
	}
	
	return counter;
}

/*
This function returns the length of the extern symbol table
*/
int symbol_table_get_extern_length() {
	return (extern_table->current_size);
}

/*
This function recieves a table and a symbol and appends it to the table 
*/
static void append_to(symbol_table* table, symbol* sym) {
	/* If table is full , reallocate */
	if(table->current_size == table->total_size) {
		table->list = (symbol**)realloc(table->list, (++(table->total_size)) * sizeof(symbol*));
		
		/* Raise error if memory failed to allocate */
		if (table == NULL) {
			raise_error(MEMORY_ERROR);
			exit(FATAL_ERROR);
		}
	}
	
	/* Append to end of table and increment current_size */
	table->list[table->current_size++] = sym;
}


static void print_table(symbol_table* table, int type_id) {
	char* name;
	int address;
	int i;
	char* types[] = {"Instructions", "Data", "External", "Entry"};

	
	
	for(i=0; i < table->current_size; i++) {
		name = utils_duplicate_string(table->list[i]->name);
		address = table->list[i]->address;
		printf("%s\t%s\t%d\n", name, types[type_id], address);
		
		free(name);
	}
}

static void free_table(symbol_table* table) {
	int i;

	
	if(table->list == NULL) return;
	
	for(i=0; i < table->current_size; i++) {
		if(table->list[i] != NULL) {
			free(table->list[i]->name);
			free(table->list[i]);
		}
	}
	
	free(table->list);
	free(table);
}

