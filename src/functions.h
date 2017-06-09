#include <stdio.h>
#define INPUT_FN_EXT "asm"
#define OUTPUT_FN_EXT "hack"
#define MAX_LINE_LENGTH 100

/* Check the command line arguments */
void init_args(int argc, char **argv);

/* Record holding for the filename information */
struct file {
	unsigned int fullname_length;
	unsigned int extension_length;
	char *fullname;
	char *extension;
	FILE *fp;
};
struct file output,input;

char *get_next_line();
