#ifndef GINK_RUNNER_H
#define GINK_RUNNER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#if defined(_WIN32)
	#include <sys/stat.h>
	#include <direct.h>
	#define stat _stat
	#define mkdir(path, mode) _mkdir(path)
	#define PATH_SEP "\\"
	#define GINK_BIN_NAME "gink.exe"
#else
	#include <sys/stat.h>
	#include <unistd.h>
	#define PATH_SEP "/"
	#define GINK_BIN_NAME "gink"
#endif

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

#define PASS(msg) printf(COLOR_GREEN "PASS: %s\n" COLOR_RESET, msg)
#define FAIL(msg) do { \
	fprintf(stderr, COLOR_RED "FAIL: %s\n" COLOR_RESET, msg); \
	exit(1); \
} while(0)
#define INFO(msg) printf(COLOR_YELLOW "%s\n" COLOR_RESET, msg)

#ifndef GINK_BIN_PATH
	#if defined(_WIN32)
		#define GINK_BIN_PATH "../gink.exe"
	#else
		#define GINK_BIN_PATH "../gink"
	#endif
#endif

#define TMP_ENV "./.gink_tmp"

static inline int file_exists(const char *path) {
	struct stat buffer;
	return (stat(path, &buffer) == 0) && (buffer.st_mode & S_IFREG);
}

static inline int copy_file(const char *src, const char *dst) {
	FILE *in = fopen(src, "rb");
	if (!in) return -1;

	FILE *out = fopen(dst, "wb");
	if (!out) {
		fclose(in);
		return -1;
	}

	char buffer[8192];
	size_t n;

	while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
		if (fwrite(buffer, 1, n, out) != n) {
			fclose(in);
			fclose(out);
			return -1;
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}

static inline void remove_dir(const char *path) {
	DIR *d = opendir(path);
	if (!d) return;

	struct dirent *entry;
	char fullpath[512];

	while ((entry = readdir(d)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		snprintf(fullpath, sizeof(fullpath), "%s%s%s", path, PATH_SEP, entry->d_name);
		remove(fullpath);
	}

	closedir(d);
	rmdir(path);
}

static inline int setup_test_env(void) {
	if (mkdir(TMP_ENV, 0755) != 0) {
		if (errno != EEXIST) return -1;
	}

	char dst_path[512];
	snprintf(dst_path, sizeof(dst_path), "%s%s%s", TMP_ENV, PATH_SEP, GINK_BIN_NAME);

	if (!file_exists(GINK_BIN_PATH)) return -1;

	if (copy_file(GINK_BIN_PATH, dst_path) != 0) return -1;

#if !defined(_WIN32)
	if (chmod(dst_path, 0755) != 0) return -1;
#endif

	if (chdir(TMP_ENV) != 0) return -1;

	return 0;
}

static inline int run_gink(const char *args) {
	char cmd[512];

#if defined(_WIN32)
	const char *prefix = "";
#else
	const char *prefix = "./";
#endif

	if (args && strlen(args) > 0)
		snprintf(cmd, sizeof(cmd), "%s%s %s", prefix, GINK_BIN_NAME, args);
	else
		snprintf(cmd, sizeof(cmd), "%s%s", prefix, GINK_BIN_NAME);

	return system(cmd);
}

static inline void teardown_test_env(void) {
	chdir("..");
	remove_dir(TMP_ENV);
}

#endif
