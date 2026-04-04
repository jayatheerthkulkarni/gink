#include "../gink_runner.h"
#include "check.h"

void check_modules() {
	INFO("Running: init creates reqter");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}
		
}

void check_init() {
	check_modules();
}
