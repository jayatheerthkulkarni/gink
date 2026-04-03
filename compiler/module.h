#ifndef MODULE_H
#define MODULE_H

#include <stdbool.h>

void compiler_check_modules(char* path);
char* get_module_from_reqter(const char* path);
bool is_valid_module_name(const char *name);

#endif
