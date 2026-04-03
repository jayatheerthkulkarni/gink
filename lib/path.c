#ifdef _WIN32
	#include <stdio.h>
	#include <direct.h>
	#define getcwd _getcwd
#else
	#include <unistd.h>
	#include <limits.h>
#endif
#include "path.h"
#include <stdlib.h>

/*
 * Gets path
 * returns null if there's error
 */
char* get_cwd() {
	char* path = getcwd(NULL, 0);
	return path;
}

long gink_getline(char **lineptr, size_t *n, FILE *stream) {
	if (!lineptr || !n || !stream) return -1;

	if (*lineptr == NULL || *n == 0) {
		*n = 128;
		*lineptr = malloc(*n);
		if (!*lineptr) return -1;
	}

	size_t pos = 0;
	int c;

	while ((c = fgetc(stream)) != EOF) {
		if (pos + 1 >= *n) {
			size_t new_size = (*n) * 2;
			char *new_ptr = realloc(*lineptr, new_size);
			if (!new_ptr) return -1;

			*lineptr = new_ptr;
			*n = new_size;
		}

		(*lineptr)[pos++] = (char)c;

		if (c == '\n') break;
	}

	if (pos == 0 && c == EOF) return -1;

	(*lineptr)[pos] = '\0';
	return (long)pos;
}
