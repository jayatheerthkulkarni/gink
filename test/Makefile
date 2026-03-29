CC = gcc
CFLAGS = -Wall -DGINK_BIN_PATH="\"$(abspath ../gink)\""
SRC = main.c cli/cli_main.c cli/init.c
OUT = test_runner

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)
