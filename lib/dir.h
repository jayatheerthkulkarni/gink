#ifndef DIR_H
#define DIR_H

#include <stdio.h>

int traverse_directory(char* path, int (*callback)(char*, FILE*));

#endif
