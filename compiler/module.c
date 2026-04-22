#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "../keyword.h"
#include "../lib/file.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#define MODULE_NAME_MAX 256

static char* trim(char *str) {
	while (*str == ' ' || *str == '\t') str++;

	if (*str == 0) return str;

	char *end = str + strlen(str) - 1;
	while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
		end--;

	*(end + 1) = '\0';
	return str;
}

bool is_valid_module_name(const char *name) {
	if (name == NULL) return false;

	size_t len = strlen(name);

	if (len == 0 || len > MODULE_NAME_MAX) return false;
	if (!isalpha((unsigned char)name[0])) return false;

	for (size_t i = 1; i < len; i++) {
		unsigned char c = (unsigned char)name[i];

		if (!(isalnum(c) || c == '_')) {
			return false;
		}
	}

	if (is_keyword(name)) return false;

	return true;
}

char* get_module_from_reqter(const char* path) {
	char reqter_path[PATH_MAX];
	snprintf(reqter_path, sizeof(reqter_path), "%s/reqter", path);

	FILE *f = fopen(reqter_path, "r");
	if (!f) return NULL;

	char *line = NULL;
	size_t len = 0;

	if (gink_getline(&line, &len, f) != -1) {
		char *p = trim(line);

		if (strncmp(p, "module ", 7) != 0) {
			goto cleanup;
		}

		char *name = p + 7;
		name = trim(name);

		char buffer[258];
		strncpy(buffer, name, sizeof(buffer) - 1);
		buffer[257] = '\0';

		if (!is_valid_module_name(buffer)) {
			const char *prefix = "module ";

			fprintf(stderr, "%s\n", reqter_path);
			fprintf(stderr, "%s%s\n", prefix, buffer);

			fprintf(stderr, "%*s", (int)strlen(prefix), "");
			for (size_t i = 0; i < strlen(buffer); i++) {
				fputc('^', stderr);
			}
			fprintf(stderr, "\n");

			fprintf(stderr, "%*sInvalid module name\n", (int)strlen(prefix), "");

			goto cleanup;
		}

		char *result = strdup(buffer);
		free(line);
		fclose(f);
		return result;
	}

cleanup:
	free(line);
	fclose(f);
	return NULL;
}

char* get_module_from_file(char* path, int* has_error) {
	if (has_error) *has_error = 0;

	FILE *f = fopen(path, "r");
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
				while (p[i] == ' ' || p[i] == '\t') i++;

				char *name = p + i;
				name = trim(name);

				size_t nlen = strlen(name);

				if (nlen == 0 || name[nlen - 1] != ';') {
					const char *prefix = "module ";

					fprintf(stderr, "%s\n", path);
					fprintf(stderr, "%s%s\n", prefix, name);

					fprintf(stderr, "%*s", (int)strlen(prefix), "");
					for (size_t j = 0; j < strlen(name); j++) {
						fputc('^', stderr);
					}
					fprintf(stderr, "\n");

					fprintf(stderr, "%*sMissing ';' after module name\n", (int)strlen(prefix), "");

					if (has_error) *has_error = 1;
					goto cleanup;
				}

				name[nlen - 1] = '\0';
				name = trim(name);

				if (*name == '\0') {
					fprintf(stderr, "%s\nInvalid module declaration (empty name)\n", path);
					if (has_error) *has_error = 1;
					goto cleanup;
				}

				char buffer[258];
				strncpy(buffer, name, sizeof(buffer) - 1);
				buffer[257] = '\0';

				if (!is_valid_module_name(buffer)) {
					const char *prefix = "module ";

					fprintf(stderr, "%s\n", path);
					fprintf(stderr, "%s%s\n", prefix, buffer);

					fprintf(stderr, "%*s", (int)strlen(prefix), "");
					for (size_t j = 0; j < strlen(buffer); j++) {
						fputc('^', stderr);
					}
					fprintf(stderr, "\n");

					fprintf(stderr, "%*sInvalid module name\n", (int)strlen(prefix), "");

					if (has_error) *has_error = 1;
					goto cleanup;
				}

				result = strdup(buffer);
				goto success;
			}

			break;
		}
	}

cleanup:
	free(line);
	fclose(f);
	return NULL;

success:
	free(line);
	fclose(f);
	return result;
}

static int check_file(char* full_path, char* reqter_module) {
	int parse_error = 0;
	char* file_module = get_module_from_file(full_path, &parse_error);
	int status = 0;

	if (parse_error) {
		status = -1;
	} else if (file_module) {
		if (!reqter_module) {
			printf("Error: file defines module but reqter missing: %s\n", full_path);
			status = -1;
		} else if (strcmp(reqter_module, file_module) != 0) {
			printf("Module mismatch:\n");
			printf("  File: %s\n", full_path);
			printf("  Expected: %s\n", reqter_module);
			printf("  Found: %s\n", file_module);
			status = -1;
		}
	}

	free(file_module);
	return status;
}

#ifndef _WIN32

int compiler_check_modules(char* path) {
	DIR *dp = opendir(path);
	if (!dp) {
		printf("Error: cannot open directory %s\n", path);
		return -1;
	}

	struct dirent *entry;
	char* reqter_module = get_module_from_reqter(path);
	int overall_status = 0;

	while ((entry = readdir(dp)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		char full_path[PATH_MAX];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR) {
			if (compiler_check_modules(full_path) == -1) {
				overall_status = -1;
			}
		} else {
			if (strcmp(entry->d_name, "reqter") == 0)
				continue;

			if (check_file(full_path, reqter_module) == -1) {
				overall_status = -1;
			}
		}
	}

	free(reqter_module);
	closedir(dp);

	return overall_status;
}

#else

int compiler_check_modules(char* path) {
	char search_path[MAX_PATH];
	snprintf(search_path, sizeof(search_path), "%s\\*", path);

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(search_path, &fd);

	if (h == INVALID_HANDLE_VALUE) {
		printf("Error: cannot open directory %s\n", path);
		return -1;
	}

	char* reqter_module = get_module_from_reqter(path);
	int overall_status = 0;

	do {
		if (!strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, ".."))
			continue;

		char full_path[MAX_PATH];
		snprintf(full_path, sizeof(full_path), "%s\\%s", path, fd.cFileName);

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (compiler_check_modules(full_path) == -1) {
				overall_status = -1;
			}
		} else {
			if (strcmp(fd.cFileName, "reqter") == 0)
				continue;

			if (check_file(full_path, reqter_module) == -1) {
				overall_status = -1;
			}
		}

	} while (FindNextFile(h, &fd));

	free(reqter_module);
	FindClose(h);

	return overall_status;
}

#endif
