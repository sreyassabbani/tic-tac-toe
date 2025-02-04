CFLAGS := $(shell sdl2-config --cflags)
LIBS := $(shell sdl2-config --libs)

BIN := bin

all: $(BIN)
	./$(BIN)

$(BIN): main.c
	gcc $(CFLAGS) main.c -o bin $(LIBS)

.PHONY: clean
clean: 
	rm -rf bin
