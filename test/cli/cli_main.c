#include "init.h"
#include "check.h"
#include "cli_main.h"

#include <stdio.h>

void run_all_cli() {
	printf("Feat: starting tests \n");
	init_init();
	check_init();
}
