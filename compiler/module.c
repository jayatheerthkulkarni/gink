#include <limits.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "../keyword.h"

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

	/** reject reserved keywords **/
	if (is_keyword(name)) return false;

	return true;
}

char* get_module_from_reqter(const char* path) {
	char reqter_path[PATH_MAX];
	snprintf(reqter_path, sizeof(reqter_path), "%s/reqter", path);

	FILE *f = fopen(reqter_path, "r");
	if (!f) {
		return NULL;
	}

	char line[1024];

	if (fgets(line, sizeof(line), f) != NULL) {
		char buffer[257];
		char extra;

		int ret = sscanf(line, "module %256s%c", buffer, &extra);

		if (ret == 2) {
			/** overflow: module name exceeds 256 characters */
			const char *prefix = "module ";

			fprintf(stderr, "%s\n", reqter_path);
			fprintf(stderr, "%s%s\n", prefix, buffer);

			fprintf(stderr, "%*s", (int)strlen(prefix), "");
			for (size_t i = 0; i < strlen(buffer); i++) {
				fputc('^', stderr);
			}
			fprintf(stderr, "\n");

			fprintf(stderr, "%*sName is too long (max 256)\n", (int)strlen(prefix), "");

			fclose(f);
			return NULL;
		}

		if (ret == 1) {
			/** valid module name */
			char *result = strdup(buffer);
			fclose(f);
			return result;
		}
	}

	/** no valid module line found */
	fclose(f);
	return NULL;
}

/*
 * This is not as strict as reqter
 * reqter is expected to have module <name> in the very
 * first line.
 * While the code files may have comments in the whole file.
 * Therefore we need to be very careful at this function.
 */

char* get_module_from_file(char* path) {
	FILE *f = fopen(path, "r");
	if (!f) {
		return NULL;
	}

	char *line = NULL;
	size_t len = 0;
	int in_block_comment = 0;

	while (getline(&line, &len, f) != -1) {
		char *p = line;

		while (*p) {
			/** handle block comments */
			if (in_block_comment) {
				if (p[0] == '*' && p[1] == '/') {
					in_block_comment = 0;
					p += 2;
				} else {
					p++;
				}
				continue;
			}

			/** start of block comment */
			if (p[0] == '/' && p[1] == '*') {
				in_block_comment = 1;
				p += 2;
				continue;
			}

			/** line comment: ignore rest of line */
			if (p[0] == '/' && p[1] == '/') {
				break;
			}

			/** skip whitespace */
			if (*p == ' ' || *p == '\t') {
				p++;
				continue;
			}

			/** empty or newline */
			if (*p == '\n' || *p == '\0') {
				break;
			}

			/** attempt parse only at first meaningful token */
			char buffer[257];
			char extra;

			int ret = sscanf(p, "module %256s%c", buffer, &extra);

			if (ret == 2) {
				/** overflow detected */
				const char *prefix = "module ";

				fprintf(stderr, "%s\n", path);
				fprintf(stderr, "%s%s\n", prefix, buffer);

				fprintf(stderr, "%*s", (int)strlen(prefix), "");
				for (size_t i = 0; i < strlen(buffer); i++) {
					fputc('^', stderr);
				}
				fprintf(stderr, "\n");

				fprintf(stderr, "%*sName is too long (max 256)\n", (int)strlen(prefix), "");

				free(line);
				fclose(f);
				return NULL;
			}

			if (ret == 1) {
				/** valid module found */
				char *result = strdup(buffer);
				free(line);
				fclose(f);
				return result;
			}

			/** not a module line */
			break;
		}
	}

	/** no module found */
	free(line);
	fclose(f);
	return NULL;
}

/* TODO: Add stat() fallback for DT_UNKNOWN when Gink takes over the world */
void compiler_check_modules(char* path) {
#ifdef _WIN32
	compiler_check_modules_windows(path);
	return;
#else

	DIR *dp = opendir(path);
	if (dp == NULL) {
		printf("Error: cannot open directory %s\n", path);
		return;
	}

	struct dirent *entry;
	char* reqter_path = get_module_from_reqter(path);

	while ((entry = readdir(dp)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			/* Gink doesn't consider . and .. files as valid */
			continue;

		char full_path[PATH_MAX];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR) {
			compiler_check_modules(full_path);
		} else {
			/*
			 * If we are here
			 * we are looking at a file.
			 */

		}
	}

	closedir(dp);
#endif

}
