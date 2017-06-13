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

			printf("%s", line);
			if (line[0] == '@') {
				int a_inst;
				sscanf(&line[1], "%d", &a_inst);
				printf(" (A-instruction: %d)", a_inst);

			}
			if (strchr(line, ';'))
				printf(" (jump instruction)");
			if (strchr(line, '='))
				printf(" (dest. instruction)");
			printf("\n");
		}
		
		free(line);
	}
	
	return 0;
}
