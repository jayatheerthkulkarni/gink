#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#include "../keyword.h"
#include "../lib/path.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#define MODULE_NAME_MAX 256

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
		char buffer[257];
		char extra;

		int ret = sscanf(line, "module %256s%c", buffer, &extra);

		if (ret == 2) {
			const char *prefix = "module ";

			fprintf(stderr, "%s\n", reqter_path);
			fprintf(stderr, "%s%s\n", prefix, buffer);

			fprintf(stderr, "%*s", (int)strlen(prefix), "");
			for (size_t i = 0; i < strlen(buffer); i++) fputc('^', stderr);
			fprintf(stderr, "\n");

			fprintf(stderr, "%*sName is too long (max 256)\n", (int)strlen(prefix), "");

			goto cleanup;
		}

		if (ret == 1) {
			char *result = strdup(buffer);
			free(line);
			fclose(f);
			return result;
		}
	}

cleanup:
	free(line);
	fclose(f);
	return NULL;
}

char* get_module_from_file(char* path) {
	FILE *f = fopen(path, "r");
	if (!f) return NULL;

	char *line = NULL;
	size_t len = 0;
	int in_block_comment = 0;

	char *result = NULL; /** FIX: move declaration here **/

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

			if (p[0] == '/' && p[1] == '/') break;

			if (*p == ' ' || *p == '\t') {
				p++;
				continue;
			}

			if (*p == '\n' || *p == '\0') break;

			char buffer[257];
			char extra;

			int ret = sscanf(p, "module %256s%c", buffer, &extra);

			if (ret == 2) {
				const char *prefix = "module ";

				fprintf(stderr, "%s\n", path);
				fprintf(stderr, "%s%s\n", prefix, buffer);

				fprintf(stderr, "%*s", (int)strlen(prefix), "");
				for (size_t i = 0; i < strlen(buffer); i++) fputc('^', stderr);
				fprintf(stderr, "\n");

				fprintf(stderr, "%*sName is too long (max 256)\n", (int)strlen(prefix), "");

				goto cleanup;
			}

			if (ret == 1) {
				result = strdup(buffer); /** assign, not declare **/
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


static void check_file(char* full_path, char* reqter_module) {
	char* file_module = get_module_from_file(full_path);

	if (file_module) {
		if (!reqter_module) {
			printf("Error: file defines module but reqter missing: %s\n", full_path);
		} else if (strcmp(reqter_module, file_module) != 0) {
			printf("Module mismatch:\n");
			printf("  File: %s\n", full_path);
			printf("  Expected: %s\n", reqter_module);
			printf("  Found: %s\n", file_module);
		}
	}

	free(file_module);
}

#ifndef _WIN32

void compiler_check_modules(char* path) {
	DIR *dp = opendir(path);
	if (!dp) {
		printf("Error: cannot open directory %s\n", path);
		return;
	}

	struct dirent *entry;
	char* reqter_module = get_module_from_reqter(path);

	while ((entry = readdir(dp)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		char full_path[PATH_MAX];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR) {
			compiler_check_modules(full_path);
		} else {
			check_file(full_path, reqter_module);
		}
	}

	free(reqter_module);
	closedir(dp);
}

#else

void compiler_check_modules(char* path) {
	char search_path[MAX_PATH];
	snprintf(search_path, sizeof(search_path), "%s\\*", path);

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(search_path, &fd);

	if (h == INVALID_HANDLE_VALUE) {
		printf("Error: cannot open directory %s\n", path);
		return;
	}

	char* reqter_module = get_module_from_reqter(path);

	do {
		if (!strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, ".."))
			continue;

		char full_path[MAX_PATH];
		snprintf(full_path, sizeof(full_path), "%s\\%s", path, fd.cFileName);

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			compiler_check_modules(full_path);
		} else {
			check_file(full_path, reqter_module);
		}

	} while (FindNextFile(h, &fd));

	free(reqter_module);
	FindClose(h);
}

#endif
