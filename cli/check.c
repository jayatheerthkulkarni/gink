#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/path.h"
#include "../compiler/module.h"

void check_modules(char* path) {
	compiler_check_modules(path);
}

int check_init(const int argc, const char* argv[]) {
	if (argc < 3) {
		printf("Error: No task specified.\n");
		return 1;
	}

	const char* flag = argv[2];

	if (!strcmp(flag, "--modules")) {
		char* path = get_cwd();
		check_modules(path);
		free(path);
	}

	return 0;
}
