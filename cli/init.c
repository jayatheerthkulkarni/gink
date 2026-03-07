#include <stdio.h>
#include "init.h"
#include "../lib/path.h"
#include "../lib/file.h"


void init_init(const int argc, const char* argv[]) {
	char* path = get_cwd();

	if(path == NULL) {
		printf("Error: couldn't figure out the path\n");
		return;
	}

	printf("Feat: Creating the reqter file\n");
	int err = create_file(path, "reqter");

	if(err == -1) {
		printf("File wasn't created due to some error\n");
		return;
	}

}
