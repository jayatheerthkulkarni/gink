#include "../gink_runner.h"
#include "init.h"

void check_reqter() {
	INFO("Running: init creates reqter");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}
	/* The below thing is exactly 256 chars */
	if (run_gink("init some_random_256_letter_just_to_check_if_the_code_is_doing_just_fine_also_I_realize_we_need_to_see_if_module_names_have_random_bullshit_like_starting_with_special_chars_and_stuff_like_that_this_is_already_207_words_wow_let_us_do_more_like_this_for_our_tests") != 0) {
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

void check_reqter_invalid_names() {
	INFO("Running: init rejects invalid module names");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	const char *invalid_names[] = {
		"if",
		"123project",
		"_project",
		"test-project",
		"test.project",
		"test@project",
		"test project",
		"!project",
		"proj$",
		"proj#",
	};

	int total = sizeof(invalid_names) / sizeof(invalid_names[0]);

	for (int i = 0; i < total; i++) {
		char command[320];

		/** quote to preserve spaces **/
		snprintf(command, sizeof(command), "init \"%s\"", invalid_names[i]);

		if (run_gink(command) == 0) {
			printf("FAILED INPUT: %s\n", invalid_names[i]);
			FAIL("init should fail for invalid module name");
			teardown_test_env();
			return;
		}

		if (file_exists("./reqter")) {
			printf("FAILED INPUT: %s\n", invalid_names[i]);
			FAIL("reqter should not be created for invalid module name");
			teardown_test_env();
			return;
		}
	}

	PASS("all invalid module names correctly rejected");
	teardown_test_env();
}

void init_init() {
	check_reqter();
	check_reqter_content();
	check_reqter_overwrite();
	check_reqter_long_module_name();
	check_reqter_invalid_names();
}
