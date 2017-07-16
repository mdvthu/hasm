#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char **argv)
{
	/* make the filename struct available */
	extern struct file input, output;
	
	/* check the args and load the filename information */
	init_args(argc, argv);

	/* open the input file for reading, or exit with an error */
	input.fp = fopen(input.fullname, "r");
	output.fp = fopen(output.fullname, "w");

	/* check file pointers are set, or exit with error */
	if (input.fp == NULL) {
		perror(input.fullname);
		exit(EXIT_FAILURE);
	} else if (output.fp == NULL) {
		perror(output.fullname);
		exit(EXIT_FAILURE);
	}

	/* Allocate memory to hold the contents of the next line */
	char *line = malloc(MAX_LINE_LENGTH+1);


	/* Build the hash table */

	/* 1. Add pre-defined symbols to the hash table */
	add_predef_symbols();

	/* 2. Fill the hash table with the labels "(LABEL)" */
	int linenumber = 0;
	while ((line = get_next_line(input.fp))) {
		if (line[0] == '(') {
			/* Remove '(' from the start of the line */
			for (int i = 0; i < strlen(line); i++)
				line[i] = line[i+1];
			/* Remove ')' from the end of the line */
			line[strlen(line)-1] = '\0';
			/* Add the label to the hash table */
			add_symbol(line, linenumber);
		} else {
			linenumber++;
		}
	}


	/* Reset the filepointer to the start of the file */
	rewind(input.fp);

	int varcount = 16;

	while ((line = get_next_line(input.fp))) {
		/* Skip line labels */
		if (line[0] == '(')
			continue;

		char *output_bin = malloc(17);

		/* Create pointers for all the processed parts of a C-instruction */
		char *dest_inst = NULL; 
		char *comp_inst = NULL;
		char *jump_inst = NULL;

		/* Set all the output bits to '0' */
		for (int i = 0; i < 16; i++)
			output_bin[i] = '0';

		/* Identify an A-instruction */
		if (line[0] == '@') {
			/* holds the decimal part of the A-instruction */
			int a_inst;
			/* get the number and check it is less than the limit */
			if (sscanf(&line[1], "%d", &a_inst) && a_inst < 32768) {
				output_bin = bin_conv(a_inst);
			} else {
				/* if the @-instruction doesn't start with a number,
				 * look it up in the hash-table */
				a_inst = lookup_symbol(&line[1]);

				/* if this fails, add it to the hash-table as a variable */
				if (a_inst != -1) {
					output_bin = bin_conv(a_inst);
				} else {
					add_symbol(&line[1], varcount);
					a_inst = varcount++;
					output_bin = bin_conv(a_inst);
				}
			}
		} else {
			/* Must represent a C-instruction */
			/* Set C-inst bits */
			output_bin[0] = '1';
			output_bin[1] = '1';
			output_bin[2] = '1';

			/* Identify and process a "dest" instruction */
			if (strchr(line, '=')) {
				dest_inst = malloc(10);
				strncpy(dest_inst, line, (strchr(line, '=') - line));

				if (strchr(dest_inst, 'M'))
					output_bin[12] = '1';
				if (strchr(dest_inst, 'D'))
					output_bin[11] = '1';
				if (strchr(dest_inst, 'A'))
					output_bin[10] = '1';
			}

			/* Identify and process "comp" */
			comp_inst = malloc(20);
			/* Copy the string after the dest instruction */
			if (strchr(line, '=')) {
				strcpy(comp_inst, (strchr(line, '=') + 1));
			} else {
				strcpy(comp_inst, line);
			}
			/* Remove the Jump instruction, if one exits */
			if (strchr(comp_inst, ';'))
				*(strchr(comp_inst, ';')) = '\0';
			/* If Comp contains M, set the A bit to 1 and change M to A */
			if (strchr(comp_inst, 'M')) {
				output_bin[3] = '1';
				*(strchr(comp_inst, 'M')) = 'A';
			}
			/* Process the "comp" part of the instruction,
			 * based on the I.S.A. */
			if (strcmp(comp_inst, "0") == 0) {
				output_bin[4] = '1';
				output_bin[6] = '1';
				output_bin[8] = '1';
			} else if (strcmp(comp_inst, "1") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[6] = '1';
				output_bin[7] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "-1") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[6] = '1';
				output_bin[8] = '1';
			} else if (strcmp(comp_inst, "D") == 0) {
				output_bin[6] = '1';
				output_bin[7] = '1';
			} else if (strcmp(comp_inst, "A") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
			} else if (strcmp(comp_inst, "!D") == 0) {
				output_bin[6] = '1';
				output_bin[7] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "!A") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "-D") == 0) {
				output_bin[6] = '1';
				output_bin[7] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "-A") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "D+1") == 0) {
				output_bin[5] = '1';
				output_bin[6] = '1';
				output_bin[7] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "A+1") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[7] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "D-1") == 0) {
				output_bin[6] = '1';
				output_bin[7] = '1';
				output_bin[8] = '1';
			} else if (strcmp(comp_inst, "A-1") == 0) {
				output_bin[4] = '1';
				output_bin[5] = '1';
				output_bin[8] = '1';
			} else if (strcmp(comp_inst, "D+A") == 0) {
				output_bin[8] = '1';
			} else if (strcmp(comp_inst, "D-A") == 0) {
				output_bin[5] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "A-D") == 0) {
				output_bin[7] = '1';
				output_bin[8] = '1';
				output_bin[9] = '1';
			} else if (strcmp(comp_inst, "D&A") == 0) {
			} else if (strcmp(comp_inst, "D|A") == 0) {
				output_bin[5] = '1';
				output_bin[7] = '1';
				output_bin[9] = '1';
			} else {
				/* If we get to this point, the instruction is invalid */
				fprintf(stderr, "Invalid comp. instruction (comp_inst: %s)\n", comp_inst);
				exit(EXIT_FAILURE);
			}


			/* Identify and process a "jump" instruction */
			if (strrchr(line, ';')) {
				jump_inst = malloc(10);
				strncpy(jump_inst, (strchr(line, ';') + 1), 11);
				if (strcmp(jump_inst, "JMP") == 0) {
					output_bin[13] = '1';
					output_bin[14] = '1';
					output_bin[15] = '1';
				} else if (strcmp(jump_inst, "JGT") == 0) {
					output_bin[15] = '1';
				} else if (strcmp(jump_inst, "JEQ") == 0) {
					output_bin[14] = '1';
				} else if (strcmp(jump_inst, "JGE") == 0) {
					output_bin[14] = '1';
					output_bin[15] = '1';
				} else if (strcmp(jump_inst, "JLT") == 0) {
					output_bin[13] = '1';
				} else if (strcmp(jump_inst, "JNE") == 0) {
					output_bin[13] = '1';
					output_bin[15] = '1';
				} else if (strcmp(jump_inst, "JLE") == 0) {
					output_bin[13] = '1';
					output_bin[14] = '1';
				} else {
					/* exit with an error if invalid */
					fprintf(stderr, "Invalid Jump-instruction/; : %s\n", line);
					exit(EXIT_FAILURE);
				}
			}

		}
		fprintf(output.fp, "%s\n", output_bin);
		free(output_bin);

	}


	/* Tidy up */
	fclose(output.fp);
	fclose(input.fp);
	free(line);
	
	return 0;
}
