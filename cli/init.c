#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include "../lib/path.h"
#include "../compiler/module.h"

#define MAX_NAME_LEN 512	/* usable characters */

int init_init(const int argc, const char* argv[]) {
	char* path = get_cwd();

	if (path == NULL) {
		printf("Error: couldn't figure out the path\n");
		return 1;
	}

	char module_name[MAX_NAME_LEN + 1];	/* +1 for null terminator */

	if (argc > 2) {
		strncpy(module_name, argv[2], MAX_NAME_LEN);
		module_name[MAX_NAME_LEN] = '\0'; /** ensure null termination **/
	} else {
		printf("Project name: ");

		if (scanf("%511s", module_name) != 1) {
			printf("Error: failed to read project name\n");
			free(path);
			return 1;
		}
	}

	/** unified validation **/
	if (!is_valid_module_name(module_name)) {
		printf("Error: invalid module name\n");
		free(path);
		return 1;
	}

	printf("Feat: Creating the reqter file\n");

	char full_path[512];
	if (snprintf(full_path, sizeof(full_path), "%s/reqter", path) >= (int)sizeof(full_path)) {
		printf("Error: path too long\n");
		free(path);
		return 1;
	}

	FILE* f = fopen(full_path, "w");

	if (f == NULL) {
		printf("Error: failed to open reqter file\n");
		free(path);
		return 1;
	}

	fprintf(f, "module %s\n", module_name);
	fclose(f);

	printf("Initialized project: %s\n", module_name);

	free(path);
	return 0;
}
