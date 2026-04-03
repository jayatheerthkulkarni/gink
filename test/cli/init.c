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

	char buffer[512];

	if (fgets(buffer, sizeof(buffer), f) == NULL) {
		FAIL("failed to read reqter content");
		fclose(f);
		teardown_test_env();
		return;
	}

	fclose(f);

	const char *expected = "module test_project\n";

	if (strcmp(buffer, expected) != 0) {
		FAIL("reqter content incorrect");
		teardown_test_env();
		return;
	}

	PASS("reqter content is correct");
	teardown_test_env();
}

void check_reqter_overwrite() {
	INFO("Running: reqter overwrite behavior");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	if (run_gink("init test_project") != 0) {
		FAIL("first init failed");
		teardown_test_env();
		return;
	}

	FILE *f = fopen("./reqter", "w");
	if (!f) {
		FAIL("failed to modify reqter");
		teardown_test_env();
		return;
	}
	fprintf(f, "corrupted content\n");
	fclose(f);

	if (run_gink("init test_project") != 0) {
		FAIL("second init failed");
		teardown_test_env();
		return;
	}

	f = fopen("./reqter", "r");
	if (!f) {
		FAIL("failed to open reqter");
		teardown_test_env();
		return;
	}

	char buffer[512];
	if (!fgets(buffer, sizeof(buffer), f)) {
		FAIL("failed to read reqter");
		fclose(f);
		teardown_test_env();
		return;
	}
	fclose(f);

	const char *expected = "module test_project\n";

	if (strcmp(buffer, expected) != 0) {
		FAIL("reqter was not overwritten correctly");
		teardown_test_env();
		return;
	}

	PASS("reqter overwrite works");
	teardown_test_env();
}

void check_reqter_long_module_name() {
	INFO("Running: init rejects module name >256 chars");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	char long_name[258];
	memset(long_name, 'a', 257);
	long_name[257] = '\0';

	char command[300];
	snprintf(command, sizeof(command), "init %s", long_name);

	if (run_gink(command) == 0) {
		FAIL("init should fail for >256 char module name");
		teardown_test_env();
		return;
	}

	if (file_exists("./reqter")) {
		FAIL("reqter should not be created for invalid module name");
		teardown_test_env();
		return;
	}

	PASS("init correctly rejects long module names");
	teardown_test_env();
}

void check_reqter_keyword_rejection() {
	INFO("Running: init rejects reserved keyword as module name");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	if (run_gink("init if") == 0) {
		FAIL("init should fail when using a reserved keyword");
		teardown_test_env();
		return;
	}

	if (file_exists("./reqter")) {
		FAIL("reqter should not be created for keyword module name");
		teardown_test_env();
		return;
	}

	PASS("init correctly rejects reserved keywords");

	teardown_test_env();
}

void init_init() {
	check_reqter();
	check_reqter_content();
	check_reqter_overwrite();
	check_reqter_long_module_name();
	check_reqter_keyword_rejection();
}
