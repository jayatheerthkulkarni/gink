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
