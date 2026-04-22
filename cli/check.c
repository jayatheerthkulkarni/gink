#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "check.h"

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
	} //else if (!strcmp(flag, "--names")) {
	// 	/*
	// 	 * By default, this works for projects with a main() function.
	// 	 * To check libraries, you must also provide the function name
	// 	 * as an additional flag after the parentheses flag in the root run.
	// 	 */
	// } else if (!strcmp(flag, "--parentheses")) {
	// 	/*
	// 	 * By default, this works for projects with a main() function.
	// 	 * To check libraries, you must also provide the function name
	// 	 * as an additional flag after the parentheses flag in the root run.
	// 	 */

	// 	int res = 0;
	// 	if (argc > 3) {
	// 		if (argc > 4) {
	// 			printf("Error: undefined amount of arguments\n");
	// 		}

	// 		const char* function = argv[3];
	// 		function += 2;

	// 		res = check_parentheses(function);
	// 	} else {
	// 		const char* function = "main";
	// 		res = check_parentheses(function);
	// 	}

	// 	return res;
	// }

	printf("Error: unknown check flag\n");
	return 1;
}
