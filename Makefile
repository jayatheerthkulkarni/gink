CC = gcc
SRC = $(shell find . -name "*.c" ! -path "./test/*")
OUT = gink
PREFIX = /usr/local/bin

all:
	$(CC) $(SRC) -o $(OUT)

install: all
	@echo "Installing $(OUT) to $(PREFIX)..."
	sudo rm -f $(PREFIX)/$(OUT)
	sudo cp $(OUT) $(PREFIX)/
	sudo chmod +x $(PREFIX)/$(OUT)
	@echo "Installation complete."

uninstall:
	@echo "Removing $(OUT) from $(PREFIX)..."
	sudo rm -f $(PREFIX)/$(OUT)
