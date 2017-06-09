#include <stdlib.h>
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
		char *line = malloc(MAX_LINE_LENGTH + 1);

		while ((line = get_next_line(input.fp)) != EOF)
			printf("%s\n", line);
	}
	
	return 0;
}
