#ifndef FILE_H
#define FILE_H

#include <stdio.h>

int create_file(const char *path, const char *filename);
long gink_getline(char **lineptr, size_t *n, FILE *stream);


#endif
