#include <string.h>

char* trim(char *str) {
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
