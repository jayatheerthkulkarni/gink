#include "./cli/cli_main.h"
#include "gink_runner.h"

int main() {
	INFO("feat: running all tests:\n");
	run_all_cli();
	INFO("All tests were completed successfully\n");
	return 0;
}
