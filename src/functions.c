#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "functions.h"
#include "uthash.h"

/* fill_filenames: fill the filename struct */
int fill_filenames(char *input_filename_arg)
{
	/* Make the struct, defined in functions.h, available */
	extern struct file input,output;
	
	/* Derive the input details from the function argument */
	input.fullname_length = strlen(input_filename_arg);
	input.extension_length = strlen(INPUT_FN_EXT);
	input.fullname = malloc(input.fullname_length + 1);
	strcpy(input.fullname, input_filename_arg);
	input.extension = malloc(input.extension_length);

	/* Calculate the output details based on input filename */
	output.extension_length = strlen(OUTPUT_FN_EXT);
	output.fullname_length = 1 + input.fullname_length - input.extension_length + output.extension_length;
	output.fullname = malloc(output.fullname_length);
	output.extension = malloc(output.extension_length);
	/* copy over the filename without the extension */
	strncpy(output.fullname, input.fullname, input.fullname_length - input.extension_length);
	/* append the extension preprocessor defined */
	strcat(output.fullname, OUTPUT_FN_EXT);

	return 0;
}

/* init_args: check the command line arguments are correct, or exit */
void init_args(int argc, char **argv) 
{
	/* 1) There should be exactly one argument provided AND 
	 * 2) The file ext. should be ".asm", AND
	 * 3) The filename provided should not exceed the system maximum */
	if (argc != 2 || strrchr(argv[1], '.') == NULL || strcmp(strrchr(argv[1], '.'), ".asm") || strlen(argv[1]) > FILENAME_MAX) {
		fprintf(stderr, "Usage: %s <filename.asm>\n", argv[0]);
		exit(EXIT_FAILURE);
	} else {
		/* If the input details are correct, fill the filename struct */
		if (fill_filenames(argv[1]))
			fprintf(stderr, "Internal error: unable to fill the filename struct\n");
	}
}

/* get_next_line: get a line of input, from a file.
 * Return a pointer to the next line 
 * (comments and whitespace removed) */
char *get_next_line(FILE *fp) 
{
	/* *line: to hold the returned line, c: getchar data, count: character counter */
	char *line = malloc(MAX_LINE_LENGTH+1);
	int c;
	int count = 0;
	while ((c = fgetc(fp))) {
		/* Return NULL pointer when end of file is reached */
		if (c == EOF)
			return NULL;
		/* Ignore tabs, spaces, and windows carriage returns */
		if (c == ' ' || c == '\t' || c == '\r')
			continue;
		/* When the newline is reached, terminate the string and
		 * exit this loop */
		if (c == '\n') {
			line[count] = '\0';
			break;
		}
		/* Match a comment:
		 * fill in the previous '/' with string terminator,
		 * advance to the end of the line,
		 * exit the loop */
		if (c == '/' && line[count-1] == '/' && count >= 1) {
			line[count-1] = '\0';
			while ((c = fgetc(fp)) != '\n');
			break;
		}
		/* Build the string */
		line[count++] = c;
	}

	/* Use recursion to remove blank lines */
	if (strlen(line) == 0)
		get_next_line(fp);
	else
		return line;
}


/* returns a 16 character string, of input in "binary": '0' and '1' characters */
char *bin_conv(int input) 
{
	char *output = malloc(17);

	/* set counter to the least significant bit for the input */
	int count = 15;

	/* Terminate the string */
	output[count+1] = '\0';

	/* Fill the output string from the right = least significant bit */
	while (input >= 1) {
		output[count--] = ((input & 1) + '0'); /* (input & 1) == input % 2 [bitwise] */
		input >>= 1; /* (input >> 1) == input / 2 */
	}
	/* Pad to the left with 0's */
	while (count >= 0)
		output[count--] = '0';

	return output;
}

/* Hash table code */
struct symbols {
    char symbol[100];             /* key */
    int id;
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct symbols *s, *tmp, *hash_table = NULL;

int add_symbol(const char *key, const int val) 
{
	s = (struct symbols*)malloc(sizeof(struct symbols));
	/* Exit if allocation fails */
	if (s == NULL)
		exit(-1);

	strncpy(s->symbol, key, 100UL);
	s->id = val;
	HASH_ADD_STR( hash_table, symbol, s);

	return 0;
}

int lookup_symbol(const char *key) 
{
	HASH_FIND_STR(hash_table, key, s);
	if (s)
		return s->id;
	else
		return -1;
}

void add_predef_symbols() 
{
	/* SP 	-> 	@0
	 * LCL 	->	@1
	 * ARG	->	@2
	 * THIS	->	@3
	 * THAT ->	@4
	 * R0 .. R15 ->	@0 .. @15
	 * SCREEN	@16384
	 * KBD	->	@24576
	 */
	add_symbol("SP", 0);
	add_symbol("LCL", 1);
	add_symbol("ARG", 2);
	add_symbol("THIS", 3);
	add_symbol("THAT", 4);
	add_symbol("R0", 0);
	add_symbol("R1", 1);
	add_symbol("R2", 2);
	add_symbol("R3", 3);
	add_symbol("R4", 4);
	add_symbol("R5", 5);
	add_symbol("R6", 6);
	add_symbol("R7", 7);
	add_symbol("R8", 8);
	add_symbol("R9", 9);
	add_symbol("R10", 10);
	add_symbol("R11", 11);
	add_symbol("R12", 12);
	add_symbol("R13", 13);
	add_symbol("R14", 14);
	add_symbol("R15", 15);
	add_symbol("SCREEN", 16384);
	add_symbol("KBD", 24576);
}


