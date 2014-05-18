#include <stdio.h>
#include <stdlib.h>

char* getString() {
		// variables
		int i = 0;
		char buf = 'X';
		char * rawIn = (char *)NULL;

		/* get string from stdin
		 * and store in dynamic array */
		while (buf != '\n' && buf != '\0') {
			buf = getchar();
			if (rawIn == (char *)NULL) {
				rawIn = (char *)malloc(sizeof(char));
			}
			else {
				rawIn = (char *)realloc(rawIn, (i + 1) * sizeof(char));
			}
			rawIn[i] = buf;
			i++;
		}
		// trim newline
		if (rawIn[i - 1] == '\n') {
			// null-terminate
			rawIn[i - 1] = '\0';
		}

		// return string
		return rawIn;
}

char* fgetString(FILE* infile) {
		// variables
		int i = 0;
		char buf = 'X';
		char * rawIn = (char *)NULL;

		/* get string from stdin
		 * and store in dynamic array */
		while (buf != '\n' && buf != '\0') {
			buf = fgetc(infile);
			if (rawIn == (char *)NULL) {
				rawIn = (char *)malloc(sizeof(char));
			}
			else {
				rawIn = (char *)realloc(rawIn, (i + 1) * sizeof(char));
			}
			rawIn[i] = buf;
			i++;
		}
		// trim newline
		if (rawIn[i - 1] != '\0') {
			// null-terminate
			rawIn[i - 1] = '\0';
		}

		// return string
		return rawIn;
}

int getInt() {
	// do not return until valid input is provided
	while (1) {
		// variables
		char* rawIn = getString();
		int out = 0;

		// ensure that input is correctly formatted & return it
		if (sscanf(rawIn, "%d", &out) == 1) {
			free(rawIn);
			return out;
		}

		// warning
		free(rawIn);
		printf("Not an Integer.\n");
	}
}
