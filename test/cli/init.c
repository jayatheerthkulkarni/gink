#include "../gink_runner.h"
#include "init.h"

void check_reqter() {
	INFO("Running: init creates reqter");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	if (run_gink("init") != 0) {
		FAIL("init command failed to execute");
		teardown_test_env();
		return;
	}

	if (!file_exists("./reqter")) {
		FAIL("init did not create reqter");
		teardown_test_env();
		return;
	}

	PASS("reqter created successfully");

	teardown_test_env();
}

void init_init() {
	check_reqter();
}
