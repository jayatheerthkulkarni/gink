#include "../gink_runner.h"
#include "init.h"

void check_reqter() {
	INFO("Running: init creates reqter");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	if (run_gink("init test_project") != 0) {
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

void check_reqter_content() {
	INFO("Running: reqter contains correct module name");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	if (run_gink("init test_project") != 0) {
		FAIL("init command failed to execute");
		teardown_test_env();
		return;
	}

	FILE* f = fopen("./reqter", "r");

	if (f == NULL) {
		FAIL("failed to open reqter file");
		teardown_test_env();
		return;
	}

	char buffer[256];

	if (fgets(buffer, sizeof(buffer), f) == NULL) {
		FAIL("failed to read reqter content");
		fclose(f);
		teardown_test_env();
		return;
	}

	fclose(f);

	if (strncmp(buffer, "module test_project", 19) != 0) {
		FAIL("reqter content incorrect");
		teardown_test_env();
		return;
	}

	PASS("reqter content is correct");

	teardown_test_env();
}

void init_init() {
	check_reqter();
	check_reqter_content();
}
