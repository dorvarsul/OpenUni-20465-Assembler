#include "main.h"
#include "error.h"
#include "reader.h"
#include "writer.h"
#include "parser.h"
#include "symbol_table.h"
#include "translator.h"
#include "image.h"

int main(int argc, char* argv[]){
	FILE* file; /* Pointer to current file being proccessed */
	int success, i; /* flag if proccess is successful, counter */
	
	/* If too few commandline arguments, exit program */
	if(argc < 2){
		raise_error(INVALID_ARGUMENTS);
		exit(FATAL_ERROR);
	}
	
	
	for(i=1; i < argc; i++){
		/* Open file */
		file = reader_open_file(argv[i], ".as");
		
		/* If file doesn't exist continue to next argument */
		if(file == NULL) continue;
		
		/* Spread macros, ignore comments and emptylines and create new .am file */
		success = parser_assemble_file(file, argv[i]);
		reader_close_file(file);
		
		/* If pre-assembly not successful go to next argument */
		if(!success)
			continue;
		
		
		symbol_table_init(); /* Initialize symbol table */
		file = reader_open_file(argv[i], ".am"); /* Open .am file */
		
		parser_first_pass(file); /* Check initial errors and symbol table */
		
		rewind(file); /* Rewind to start of the file */
		image_init(); /* Initialize instructions and data images */
		
		/* Check complex errors and create initial translation to binary */
		success = parser_second_pass(file, argv[i]);
		
		/* If both passes are without errors than write files */
		if (success) {
			fprintf(stdout, "Success!\n\n");
			writer_write_output_files(argv[i]);
			fprintf(stdout, "Finished!\n");
		}else {
			fprintf(stdout, "Failed!\n");
		}
		
		/* Free allocated structures and close file pointer */
		image_free();
		reader_close_file(file);
		symbol_table_free();
	}
	
	
	return 0;
}