#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "./cli/cli_main.h"
#include "gink_runner.h"

char* real_path = NULL;

int main() {
	INFO("feat: running all tests:\n");
	setup();
	run_all_cli();
	INFO("All tests were completed successfully\n");
	return 0;
}
