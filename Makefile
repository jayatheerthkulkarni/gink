CC = gcc
SRC = $(shell find . -name "*.c")

all:
	$(CC) $(SRC) -o gink
