#include "image.h"
#include "constants.h"
#include "translator.h"
#include <string.h>
#include <stdlib.h>

typedef struct Instruction {
	int size; /* Size of encoded words of instructions */
	char** instructions; /* Pointer to encoded words */
} instruction;

typedef struct Data {
	int size; /* Size of encoded words of data */
	char** data; /* Pointer to encoded words */
} data;

typedef struct Image {
	struct Instruction** ic_arr; /* List of instruction blocks */
	int ic_curr_size; /* Size of initialized instructions */
	int ic_total_size; /* Total size allocated */
	
	struct Data** data_arr; /* List of data blocks */
	int data_curr_size; /* Size of initialized data blocks */
	int data_total_size; /* Total size allocated */
} image;


image* img; /* Global pointer to the image data structure. */


/* Initializes the image data structure for instructions and data. */
void image_init() {
	/* Init total image */
	img = (image*)malloc(sizeof(image));
	
	/* Init instructions section */
	img->ic_arr = (instruction**)malloc(sizeof(instruction*) * TABLE_BASE_SIZE);
	img->ic_curr_size = 0;
	img->ic_total_size = TABLE_BASE_SIZE;
	
	/* Init data section */
	img->data_arr = (data**)malloc(sizeof(data*) * TABLE_BASE_SIZE);
	img->data_curr_size = 0;
	img->data_total_size = TABLE_BASE_SIZE;
}


/* Frees the memory allocated for the image data structure and its contents. */
void image_free() {
	int i, j;
	
	if(img == NULL) return;
	
	for(i=0; i < img->ic_curr_size; i++) {
		for(j=0; j < img->ic_arr[i]->size; j++) {
			free(img->ic_arr[i]->instructions[j]);
		}
		free(img->ic_arr[i]->instructions);
		free(img->ic_arr[i]);
	}
	
	for(i=0; i < img->data_curr_size; i++) {
		for(j=0; j < img->data_arr[i]->size; j++) {
			free(img->data_arr[i]->data[j]);
		}
		free(img->data_arr[i]->data);
		free(img->data_arr[i]);
	}
	
	free(img->ic_arr);
	free(img->data_arr);
	free(img);
}

/*
 Prints the contents of the image, including instructions and data.
 For debug purposes 
 */

void image_print() {
	int i,j;
	int counter = 0;
	
	if(img == NULL) return;
	

	printf("Printing Image\n\n");
	for(i=0; i < img->ic_curr_size; i++) {
		for(j=0; j < img->ic_arr[i]->size; j++) {
			printf("%d\t%s\n", counter++, img->ic_arr[i]->instructions[j]);
		}
	}
	
	for(i=0; i < img->data_curr_size; i++) {
		printf("\nData index: %d\n", i);
		for(j=0; j < img->data_arr[i]->size; j++) {
			printf("%d\t%s\n", counter++, img->data_arr[i]->data[j]);
		}
	}
}

/*
Appends a list of binary instructions to the image's instruction array.
*/

void image_append_to_instructions(char** coding, int amount_of_lines) {
	/* Initialize instruction block and assign values */
	instruction* ic = (instruction*)malloc(sizeof(instruction));
	ic->size = amount_of_lines;
	ic->instructions = coding;
	
	/* If instruction array is full than reallocate memory */
	if (img->ic_curr_size == img->ic_total_size) {
		img->ic_arr = (instruction**)realloc(img->ic_arr, img->ic_total_size + TABLE_BASE_SIZE);
		img->ic_total_size+= TABLE_BASE_SIZE;
	}
	
	/* Assign instruction to end of instruction array */
	img->ic_arr[img->ic_curr_size] = ic;
	img->ic_curr_size++;
}

/*
 * Appends a list of data values to the image's data array.
 */
void image_append_to_data(char** coding, int amount_of_lines) {
	/* Initialize data block and assign values */
	data* data_node = (data*)malloc(sizeof(data));
	data_node->size = amount_of_lines;
	data_node->data = coding;
	
	/* Reallocate if data array is full */
	if(img->data_curr_size == img->data_total_size) {
		img->data_arr = (data**)realloc(img->data_arr, img->data_total_size + TABLE_BASE_SIZE);
		img->data_total_size+= TABLE_BASE_SIZE;
	}
	
	/* Assign data to end of data array */
	img->data_arr[img->data_curr_size] = data_node;
	img->data_curr_size++;
}

/*
 * Translates the image's instructions and data into a
 * base64 format and writes it to a file.
 */
void image_translate(FILE* ob_file) {
	int i,j;
	
	/* For each line of instruction binary word, translate it and write it to final .ob file */
	for(i=0; i < img->ic_curr_size; i++) {
		for(j=0; j < img->ic_arr[i]->size; j++) {
			translator_translate_word(ob_file, img->ic_arr[i]->instructions[j]);
		}
	}
	
	/* For each of line of binary word of data, translate it and write it to final .ob file*/
	for(i=0; i < img->data_curr_size; i++) {
		for(j=0; j < img->data_arr[i]->size; j++) {
			translator_translate_word(ob_file, img->data_arr[i]->data[j]);
		}
	}
}