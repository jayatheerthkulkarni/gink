#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/path.h"
#include "../compiler/module.h"

int check_modules(char* path) {
	return compiler_check_modules(path);
}

int check_init(const int argc, const char* argv[]) {
	if (argc < 3) {
		printf("Error: No task specified.\n");
		return 1;
	}

	const char* flag = argv[2];

	if (!strcmp(flag, "--modules")) {
		char* path = get_cwd();

		if (path == NULL) {
			printf("Error: failed to get current directory\n");
			return 1;
		}

		int result = check_modules(path);
		free(path);
		return result;
	}

	printf("Error: unknown check flag\n");
	return 1;
}
