#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char **argv)
{
	/* check the args and load the filename information */
	init_args(argc, argv);

	/* make the filename struct available */
	extern struct file input, output;

	/* open the input file for reading, or exit with an error */
	input.fp = fopen(input.fullname, "r");
	if (input.fp == NULL) {
		perror(input.fullname);
		exit(EXIT_FAILURE);
	} else {
		char *line = malloc(MAX_LINE_LENGTH+1);
		
		while ((line = get_next_line(input.fp))) {

			char *output_bin = malloc(17);
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
					/* exit with an error if invalid */
					fprintf(stderr, "Invalid A-instruction: %s\n", line);
					exit(EXIT_FAILURE);
				}
			} else {
				/* Must represent a C-instruction */
				/* Set C-inst bits */
				output_bin[0] = '1';
				output_bin[1] = '1';
				output_bin[2] = '1';

				char *comp_inst = NULL;
				char *dest_inst = NULL; 
				char *jump_inst = NULL;
				
				/* Identify and process a "dest" instruction */
				if (strchr(line, '=')) {
					dest_inst = malloc(10);
					strncpy(dest_inst, line, strchr(line, '=') - line);
					if (strchr(dest_inst, 'M'))
						output_bin[12] = '1';
					if (strchr(dest_inst, 'D'))
						output_bin[11] = '1';
					if (strchr(dest_inst, 'A'))
						output_bin[10] = '1';
				}

				/* Identify and process a "jump" instruction */
				if (strrchr(line, ';')) {
					jump_inst = malloc(10);
					strncpy(jump_inst, strchr(line, ';'), 11);
					if (strcmp(jump_inst, ";JMP") == 0) {
						output_bin[13] = '1';
						output_bin[14] = '1';
						output_bin[15] = '1';
					} else if (strcmp(jump_inst, ";JGT") == 0) {
						output_bin[15] = '1';
					} else if (strcmp(jump_inst, ";JEQ") == 0) {
						output_bin[14] = '1';
					} else if (strcmp(jump_inst, ";JGE") == 0) {
						output_bin[14] = '1';
						output_bin[15] = '1';
					} else if (strcmp(jump_inst, ";JLT") == 0) {
						output_bin[13] = '1';
					} else if (strcmp(jump_inst, ";JNE") == 0) {
						output_bin[13] = '1';
						output_bin[15] = '1';
					} else if (strcmp(jump_inst, ";JLE") == 0) {
						output_bin[13] = '1';
						output_bin[14] = '1';
					} else {
						/* exit with an error if invalid */
						fprintf(stderr, "Invalid Jump-instruction/; : %s\n", line);
						exit(EXIT_FAILURE);
					}


				}

			}
			printf("%s (%s)\n", output_bin, line);
			free(output_bin);
			
		}

		
		free(line);
	}
	
	return 0;
}
