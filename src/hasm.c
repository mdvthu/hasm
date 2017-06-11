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
		int count = 1;
		while (line = get_next_line(input.fp))
			printf("\nline %2d: %s (length: %zd)\n", count++, line, strlen(line));
	}
	
	return 0;
}
