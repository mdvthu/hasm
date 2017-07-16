#include <stdio.h>
#include "uthash.h"

#define INPUT_FN_EXT "asm"
#define OUTPUT_FN_EXT "hack"
#define MAX_LINE_LENGTH 100
#define INSTRUCTION_LENGTH 16

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

/* Get the next line containing valid code, from a filepointer reference */
char *get_next_line(FILE *fp);

/* Convert an input int. to a char. string representing the binary value */
char *bin_conv(int input);

/* Add all the pre-defined symbols to the hash-table */
void add_predef_symbols();

int add_symbol(const char *key, const int val);

int lookup_symbol(const char *key);

