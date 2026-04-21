#include "../gink_runner.h"
#include "check.h"

void check_modules() {
	INFO("Running: check --modules validates nested modules");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	/* create package directory */
	create_dir("temp");

	if (cd("temp") != 0) {
		FAIL("failed to enter temp directory");
		teardown_test_env();
		return;
	}

	/* initialize root module */
	if (run_gink("init temp") != 0) {
		FAIL("failed to init root module");
		teardown_test_env();
		return;
	}

	/* valid file inside root package */
	write_file(
		"main.gink",
		"module temp;\n"
		"main() {\n"
		"}\n"
	);

	/* nested child package */
	create_dir("child");

	if (cd("child") != 0) {
		FAIL("failed to enter child directory");
		teardown_test_env();
		return;
	}

	if (run_gink("init child") != 0) {
		FAIL("failed to init child module");
		teardown_test_env();
		return;
	}

	write_file(
		"lib.gink",
		"module child;\n"
	);

	/* back to temp */
	if (cd("..") != 0) {
		FAIL("failed to leave child directory");
		teardown_test_env();
		return;
	}

	/* run module checker */
	if (run_gink("check --modules") != 0) {
		FAIL("check --modules failed");
		teardown_test_env();
		return;
	}

	PASS("check --modules passed on valid nested modules");

	/* back to tmp root */
	if (cd("..") != 0) {
		FAIL("failed to leave temp directory");
		teardown_test_env();
		return;
	}

	teardown_test_env();
}

#include "../gink_runner.h"
#include "check.h"

/*
 * Weird malformed syntax test
 * Expected: command fails because semicolon missing
 */
void check_modules_weird_expected_fail() {
	INFO("Running: weird malformed module declaration should fail");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	create_dir("odd");
	cd("odd");

	if (run_gink("init odd") != 0) {
		FAIL("init failed");
		teardown_test_env();
		return;
	}

	write_file(
		"broken.gink",
		"/* chaos */\n"
		"// random stuff\n"
		"module odd\n"     /* missing ; intentionally */
		"main(){}\n"
	);

	if (run_gink("check --modules") == 0) {
		FAIL("checker should fail on malformed module syntax");
		teardown_test_env();
		return;
	}

	PASS("weird malformed syntax correctly failed");

	cd("..");
	teardown_test_env();
}


/*
 * Extreme: module name exactly 256 chars
 * Expected: pass
 */
void check_modules_max_name_boundary() {
	INFO("Running: max boundary module name (256 chars)");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	char name[257];
	name[0] = 'a';

	for (int i = 1; i < 256; i++)
		name[i] = 'b';

	name[256] = '\0';

	char cmd[320];
	snprintf(cmd, sizeof(cmd), "init %s", name);

	if (run_gink(cmd) != 0) {
		FAIL("init failed for valid 256-char name");
		teardown_test_env();
		return;
	}

	if (run_gink("check --modules") != 0) {
		FAIL("module checker failed on 256-char boundary");
		teardown_test_env();
		return;
	}

	PASS("256-char boundary handled correctly");
	teardown_test_env();
}


/*
 * Extreme: deeply nested directories
 * Expected: recursion works
 */
void check_modules_deep_nesting() {
	INFO("Running: deep nested directory recursion");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	create_dir("a");
	cd("a");
	run_gink("init a");
	write_file("a.gink", "module a;\n");

	create_dir("b");
	cd("b");
	run_gink("init b");
	write_file("b.gink", "module b;\n");

	create_dir("c");
	cd("c");
	run_gink("init c");
	write_file("c.gink", "module c;\n");

	create_dir("d");
	cd("d");
	run_gink("init d");
	write_file("d.gink", "module d;\n");

	if (run_gink("check --modules") != 0) {
		FAIL("checker failed in deep nesting");
		teardown_test_env();
		return;
	}

	PASS("deep recursion works");

	cd("..");
	cd("..");
	cd("..");
	cd("..");

	teardown_test_env();
}


/*
 * Extreme weirdness:
 * comments + spaces + tabs before module
 * Expected: parser still succeeds
 */
void check_modules_comment_noise() {
	INFO("Running: comment noise before module");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	run_gink("init noise");

	write_file(
		"noise.gink",
		"/* start */\n"
		"// comment line\n"
		"\t   module noise; \n"
		"main(){}\n"
	);

	if (run_gink("check --modules") != 0) {
		FAIL("checker failed with comment noise");
		teardown_test_env();
		return;
	}

	PASS("comment noise handled");
	teardown_test_env();
}


/*
 * Extreme fail:
 * mismatch module names
 */
void check_modules_mismatch_fail() {
	INFO("Running: mismatch module should fail");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	run_gink("init realname");

	write_file(
		"fake.gink",
		"module impostor;\n"
	);

	if (run_gink("check --modules") == 0) {
		FAIL("checker should fail on mismatch");
		teardown_test_env();
		return;
	}

	PASS("mismatch correctly failed");
	teardown_test_env();
}

void check_modules_name_over_256_fail() {
	INFO("Running: module name >256 should fail");

	if (setup_test_env() != 0) {
		FAIL("failed to setup test environment");
		return;
	}

	/* valid package first */
	if (run_gink("init validpkg") != 0) {
		FAIL("init failed");
		teardown_test_env();
		return;
	}

	/* create invalid module name: 257 chars */
	char long_name[258];

	long_name[0] = 'a';
	for (int i = 1; i < 257; i++)
		long_name[i] = 'b';

	long_name[257] = '\0';

	char content[512];
	snprintf(
		content,
		sizeof(content),
		"module %s;\n"
		"main(){}\n",
		long_name
	);

	write_file("overflow.gink", content);

	if (run_gink("check --modules") == 0) {
		FAIL("checker should fail for module name >256");
		teardown_test_env();
		return;
	}

	PASS("module name >256 correctly rejected");
	teardown_test_env();
}

void check_init() {
	check_modules();
	check_modules_weird_expected_fail();
	check_modules_max_name_boundary();
	check_modules_deep_nesting();
	check_modules_comment_noise();
	check_modules_mismatch_fail();
	check_modules_name_over_256_fail();
}
