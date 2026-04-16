CC = gcc
WINCC = x86_64-w64-mingw32-gcc
MUSLCC = musl-gcc

SRC = $(shell find . -name "*.c" ! -path "./test/*")
OUT = gink

PREFIX ?= /usr/local/bin

all: linux

linux:
	@echo "Building Linux binary..."
	$(CC) $(SRC) -O2 -s -o $(OUT)
	@echo "Built: $(OUT)"

linux-static:
	@echo "Building static Linux binary (musl)..."
	$(MUSLCC) $(SRC) -O2 -static -s -o $(OUT)
	@echo "Built: $(OUT)"

windows:
	@echo "Building Windows executable..."
	$(WINCC) $(SRC) -O2 -static -s -o $(OUT).exe
	@echo "Built: $(OUT).exe"

install: linux
	@echo "Installing $(OUT) to $(PREFIX)..."
	mkdir -p "$(PREFIX)"
	cp $(OUT) "$(PREFIX)/$(OUT)"
	chmod +x "$(PREFIX)/$(OUT)"
	@echo "Installation complete."

uninstall:
	@echo "Removing $(OUT) from $(PREFIX)..."
	rm -f "$(PREFIX)/$(OUT)"
	@echo "Uninstall complete."

clean:
	rm -f $(OUT) $(OUT).exe

release: clean linux linux-static windows
	@echo "Release build complete."
