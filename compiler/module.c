#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "../keyword.h"
#include "../lib/file.h"
#include "../lib/dir.h"

#define MODULE_NAME_MAX 256

static char* trim(char *str) {
	while (*str == ' ' || *str == '\t')
		str++;

	if (*str == '\0')
		return str;

	char *end = str + strlen(str) - 1;

	while (end > str &&
	      (*end == ' ' || *end == '\t' ||
	       *end == '\n' || *end == '\r'))
		end--;

	end[1] = '\0';
	return str;
}

bool is_valid_module_name(const char *name) {
	if (!name)
		return false;

	size_t len = strlen(name);

	if (len == 0 || len > MODULE_NAME_MAX)
		return false;

	if (!isalpha((unsigned char)name[0]))
		return false;

	for (size_t i = 1; i < len; i++) {
		unsigned char c = (unsigned char)name[i];

		if (!(isalnum(c) || c == '_'))
			return false;
	}

	if (is_keyword(name))
		return false;

	return true;
}

char* get_module_from_reqter(const char *path) {
	char reqter_path[PATH_MAX];
	snprintf(reqter_path, sizeof(reqter_path), "%s/reqter", path);

	FILE *f = fopen(reqter_path, "r");
	if (!f)
		return NULL;

	char *line = NULL;
	size_t len = 0;
	char *result = NULL;

	if (gink_getline(&line, &len, f) != -1) {
		char *p = trim(line);

		if (!strncmp(p, "module ", 7)) {
			char *name = trim(p + 7);

			char buffer[MODULE_NAME_MAX + 2];
			strncpy(buffer, name, sizeof(buffer) - 1);
			buffer[sizeof(buffer) - 1] = '\0';

			if (is_valid_module_name(buffer))
				result = strdup(buffer);
		}
	}

	free(line);
	fclose(f);
	return result;
}

char* get_module_from_file(FILE *f, int *has_error) {
	if (has_error)
		*has_error = 0;

	if (!f)
		return NULL;

	char *line = NULL;
	size_t len = 0;
	int in_block_comment = 0;
	char *result = NULL;

	while (gink_getline(&line, &len, f) != -1) {
		char *p = line;

		while (*p) {
			if (in_block_comment) {
				if (p[0] == '*' && p[1] == '/') {
					in_block_comment = 0;
					p += 2;
				} else {
					p++;
				}
				continue;
			}

			if (p[0] == '/' && p[1] == '*') {
				in_block_comment = 1;
				p += 2;
				continue;
			}

			if (p[0] == '/' && p[1] == '/')
				break;

			if (*p == ' ' || *p == '\t') {
				p++;
				continue;
			}

			if (*p == '\n' || *p == '\0')
				break;

			if (!strncmp(p, "module", 6) &&
			    (p[6] == ' ' || p[6] == '\t')) {

				int i = 6;

				while (p[i] == ' ' || p[i] == '\t')
					i++;

				char *name = trim(p + i);
				size_t nlen = strlen(name);

				if (nlen == 0 || name[nlen - 1] != ';') {
					if (has_error)
						*has_error = 1;
					goto cleanup;
				}

				name[nlen - 1] = '\0';
				name = trim(name);

				if (*name == '\0') {
					if (has_error)
						*has_error = 1;
					goto cleanup;
				}

				char buffer[MODULE_NAME_MAX + 2];
				strncpy(buffer, name, sizeof(buffer) - 1);
				buffer[sizeof(buffer) - 1] = '\0';

				if (!is_valid_module_name(buffer)) {
					if (has_error)
						*has_error = 1;
					goto cleanup;
				}

				result = strdup(buffer);
				goto cleanup;
			}

			break;
		}
	}

cleanup:
	free(line);
	return result;
}

static int check_file(char *full_path, FILE *f) {
	if (!f) {
		printf("Error: couldn't validate the file\n");
		return -1;
	}

	/* skip reqter */
	const char *base = strrchr(full_path, '/');

#ifdef _WIN32
	const char *base2 = strrchr(full_path, '\\');
	if (!base || (base2 && base2 > base))
		base = base2;
#endif

	base = base ? base + 1 : full_path;

	if (!strcmp(base, "reqter"))
		return 0;

	char dir_path[PATH_MAX];
	strncpy(dir_path, full_path, sizeof(dir_path) - 1);
	dir_path[sizeof(dir_path) - 1] = '\0';

	char *slash = strrchr(dir_path, '/');

#ifdef _WIN32
	char *slash2 = strrchr(dir_path, '\\');
	if (!slash || (slash2 && slash2 > slash))
		slash = slash2;
#endif

	if (slash)
		*slash = '\0';
	else
		strcpy(dir_path, ".");

	int parse_error = 0;

	char *file_module =
		get_module_from_file(f, &parse_error);

	char *reqter_module =
		get_module_from_reqter(dir_path);

	int status = 0;

	if (parse_error) {
		status = -1;
		goto cleanup;
	}

	if (file_module) {
		if (!reqter_module) {
			printf("Error: file defines module but reqter missing: %s\n",
			       full_path);
			status = -1;
		} else if (strcmp(reqter_module, file_module) != 0) {
			printf("Module mismatch:\n");
			printf("  File: %s\n", full_path);
			printf("  Expected: %s\n", reqter_module);
			printf("  Found: %s\n", file_module);
			status = -1;
		}
	}

cleanup:
	free(file_module);
	free(reqter_module);

	return status;
}

int compiler_check_modules(char *path) {
	int res = traverse_directory(path, check_file);

	if (res != 0)
		return -1;

	return 0;
}
