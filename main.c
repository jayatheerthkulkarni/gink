#include "cli/init.h"
#include <stdio.h>
#include <string.h>

/*
 * This file primarily maintains a global struct which is passed to all the
 * commands.
 */

/*
 * Error protocol
 * 0 - No errors
 * -1 - Insufficient args
 */

/*
 * Parses the initial cmd arguments and sends them to ./cli/<argument> folder.
 */
int main(const int argc, const char* argv[]) {
	if(argc < 2) {
		printf("Error: Not enough arguments");
		return -1;
	}
	const char* subcommand = argv[1];


	if(!strcmp(subcommand, "init")) {
		init_init(argc, argv);
	}


}
