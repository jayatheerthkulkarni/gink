#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

/*
 * Creates a file at path/filename
 *
 * returns:
 * 0  -> success
 * -1 -> error
 */
int create_file(const char *path, const char *filename) {

	size_t len = strlen(path) + strlen(filename) + 2;
	char *fullpath = malloc(len);

	if (!fullpath)
		return -1;

	snprintf(fullpath, len, "%s%s%s", path, PATH_SEP, filename);

	FILE *fp = fopen(fullpath, "w");

	free(fullpath);

	if (fp == NULL)
		return -1;

	fclose(fp);
	return 0;
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
