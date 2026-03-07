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
