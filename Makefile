CC = gcc
SRC = $(shell find . -name "*.c" ! -path "./test/*")
OUT = gink

all:
	$(CC) $(SRC) -o $(OUT)
