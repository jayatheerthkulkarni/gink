#ifndef GINK_RUNNER_H
#define GINK_RUNNER_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#if defined(_WIN32)
	#include <sys/stat.h>
	#include <direct.h>
	#define stat _stat
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

extern char* real_path;

static inline int gink_mkdir(const char *path) {
#if defined(_WIN32)
	return _mkdir(path);
#else
	return mkdir(path, 0755);
#endif
}

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
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		snprintf(fullpath, sizeof(fullpath), "%s%s%s",
			path, PATH_SEP, entry->d_name);

		struct stat st;

		if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode))
			remove_dir(fullpath);
		else
			remove(fullpath);
	}

	closedir(d);
	rmdir(path);
}

static inline int cd(const char *path) {
	if (!path || !*path) {
		errno = EINVAL;
		return -1;
	}

#if defined(_WIN32)
	return _chdir(path);
#else
	return chdir(path);
#endif
}

static inline void setup(void) {
	if (gink_mkdir(TMP_ENV) != 0 && errno != EEXIST)
		FAIL("failed to create temp root");

	char dst[512];

	snprintf(dst, sizeof(dst), "%s%s%s",
		TMP_ENV, PATH_SEP, GINK_BIN_NAME);

	if (!file_exists(GINK_BIN_PATH))
		FAIL("gink binary missing");

	if (copy_file(GINK_BIN_PATH, dst) != 0)
		FAIL("failed to copy gink binary");

#if !defined(_WIN32)
	if (chmod(dst, 0755) != 0)
		FAIL("chmod failed");
#endif

#if defined(_WIN32)
	#ifndef _MAX_PATH
	#define _MAX_PATH 260
	#endif
	real_path = _fullpath(NULL, dst, _MAX_PATH);
#else
	real_path = realpath(dst, NULL);
#endif

	if (!real_path)
		FAIL("failed resolving absolute path");

	PASS("global setup complete");
}

static inline int setup_test_env(void) {
	remove_dir("./work");
	gink_mkdir("./work");
	return cd("./work");
}

static inline void teardown_test_env(void) {
	cd("..");
	remove_dir("./work");
}

static inline int run_gink(const char *args) {
	char cmd[1024];

	if (!real_path)
		return -1;

	if (args && strlen(args) > 0)
		snprintf(cmd, sizeof(cmd), "\"%s\" %s", real_path, args);
	else
		snprintf(cmd, sizeof(cmd), "\"%s\"", real_path);

	return system(cmd);
}

static inline void teardown(void) {
	if (real_path) {
		free(real_path);
		real_path = NULL;
	}

	remove_dir(TMP_ENV);
	PASS("global teardown complete");
}

static inline void write_file(const char* name, const char* content) {
	FILE* f = fopen(name, "w");
	if (f) {
		fputs(content, f);
		fclose(f);
	}
}

static inline void create_dir(const char* name) {
	gink_mkdir(name);
}

#endif
