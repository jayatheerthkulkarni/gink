#include "cli/init.h"
#include "cli/check.h"
#include <stdio.h>
#include <string.h>

int main(const int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Error: Not enough arguments\n");
		return 1;
	}

	const char* subcommand = argv[1];

	if (!strcmp(subcommand, "init")) {
		return init_init(argc, argv);
	}

	if (!strcmp(subcommand, "check")) {
		return check_init(argc, argv);
	}

	printf("Error: unknown command\n");
	return 1;
}
