#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "functions.h"

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

/* bin_conv: copies an input string (in decimal) to
 * dest. char string (in "binary" -- '0' and '1' characters) */
void bin_conv(char destination[16], char *input_str)
{
	long source_int = strtol(input_str, (char **)input_str + strlen(input_str), 10);
	
	destination[16] = '\0';
	int bit_position = 15;

	while (bit_position >= 1) {
		while (source_int > 0) {	
			destination[bit_position--] = (source_int % 2) + '0';
			source_int /= 2;
		}
		destination[bit_position--] = '0';
	}
}

/* check_inst_type: analyse the line for the instruction type */
char check_inst_type(char *line)
{	
	if (line[0] == '@' && isdigit(line[1]))
		return 'A';
	else
		return '\0';
}
