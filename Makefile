CC = gcc
SRC = $(shell find . -name "*.c" -not -path "./test/*")

all:
	$(CC) $(SRC) -o gink
