CFLAGS := $(shell sdl2-config --cflags)
LIBS := $(shell sdl2-config --libs) -lSDL2_ttf

BIN := bin

# Phony targets
.PHONY: clean all

# Default target: run the binary
all: $(BIN)
	./$(BIN)

# Link the binary
$(BIN): main.c
	gcc $(CFLAGS) main.c -o $(BIN) $(LIBS)

clean: 
	rm -rf bin
