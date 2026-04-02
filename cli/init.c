#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include "../lib/path.h"
#include "../keyword.h"

#define MAX_NAME_LEN 256

int init_init(const int argc, const char* argv[]) {
	char* path = get_cwd();

	if (path == NULL) {
		printf("Error: couldn't figure out the path\n");
		return 1;
	}

	char module_name[MAX_NAME_LEN];

	if (argc > 2) {
		if (strlen(argv[2]) >= MAX_NAME_LEN) {
			printf("Error: module name too long (max %d characters)\n", MAX_NAME_LEN - 1);
			free(path);
			return 1;
		}
		strcpy(module_name, argv[2]);
		if (is_keyword(module_name)) {
			printf("Error: '%s' is a reserved keyword\n", module_name);
			free(path);
			return 1;
		}
	} else {
		printf("Project name: ");
		if (scanf("%255s", module_name) != 1) {
			printf("Error: failed to read project name\n");
			free(path);
			return 1;
		}
	}

	printf("Feat: Creating the reqter file\n");

	char full_path[512];
	snprintf(full_path, sizeof(full_path), "%s/reqter", path);

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
