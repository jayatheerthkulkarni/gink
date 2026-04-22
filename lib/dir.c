#include <string.h>
#include "dir.h"

#ifdef _WIN32
#include <windows.h>

int traverse_directory(char* path, int (*callback)(char*, FILE*)) {
	char search_path[MAX_PATH];
	snprintf(search_path, sizeof(search_path), "%s\\*", path);

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(search_path, &fd);

	if (h == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Error: cannot open directory %s\n", path);
		return -1;
	}

	int overall_status = 0;

	do {
		if (!strcmp(fd.cFileName, ".") ||
		    !strcmp(fd.cFileName, ".."))
			continue;

		char full_path[MAX_PATH];
		snprintf(full_path, sizeof(full_path),
		         "%s\\%s", path, fd.cFileName);

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (traverse_directory(full_path, callback) == -1)
				overall_status = -1;
		} else {
			FILE *f = fopen(full_path, "r");

			if (f) {
				if (callback(full_path, f) == -1)
					overall_status = -1;

				fclose(f);
			} else {
				fprintf(stderr,
				        "Error: cannot open file %s\n",
				        full_path);
				overall_status = -1;
			}
		}

	} while (FindNextFile(h, &fd));

	FindClose(h);
	return overall_status;
}

#else

#include <dirent.h>
#include <limits.h>

int traverse_directory(char* path, int (*callback)(char*, FILE*)) {
	DIR *dp = opendir(path);

	if (!dp) {
		fprintf(stderr,
		        "Error: cannot open directory %s\n",
		        path);
		return -1;
	}

	struct dirent *entry;
	int overall_status = 0;

	while ((entry = readdir(dp)) != NULL) {
		if (!strcmp(entry->d_name, ".") ||
		    !strcmp(entry->d_name, ".."))
			continue;

		char full_path[PATH_MAX];
		snprintf(full_path, sizeof(full_path),
		         "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR) {
			if (traverse_directory(full_path, callback) == -1)
				overall_status = -1;
		} else {
			FILE *f = fopen(full_path, "r");

			if (f) {
				if (callback(full_path, f) == -1)
					overall_status = -1;

				fclose(f);
			} else {
				fprintf(stderr,
				        "Error: cannot open file %s\n",
				        full_path);
				overall_status = -1;
			}
		}
	}

	closedir(dp);
	return overall_status;
}

#endif
