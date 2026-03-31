#include <stdio.h>
#include <string.h>
#include "init.h"
#include "../lib/path.h"
#include "../lib/file.h"

#define MAX_NAME_LEN 256

void init_init(const int argc, const char* argv[]) {
	char* path = get_cwd();

	if(path == NULL) {
		printf("Error: couldn't figure out the path\n");
		return;
	}

	char module_name[MAX_NAME_LEN];

	if(argc > 2) {
		strncpy(module_name, argv[2], MAX_NAME_LEN - 1);
		module_name[MAX_NAME_LEN - 1] = '\0';
	} else {
		printf("Project name: ");
		if(scanf("%255s", module_name) != 1) {
			printf("Error: failed to read project name\n");
			return;
		}
	}

	printf("Feat: Creating the reqter file\n");

	int err = create_file(path, "reqter");

	if(err == -1) {
		printf("Error: cannot create the reqter file\n");
		return;
	}

	FILE* f = fopen("reqter", "w");

	if(f == NULL) {
		printf("Error: failed to open reqter file\n");
		return;
	}

	fprintf(f, "module %s\n", module_name);
	fclose(f);

	printf("Initialized project: %s\n", module_name);
}
