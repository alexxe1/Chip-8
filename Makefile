CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)
SRC = src/main.c src/chip8.c src/platform.c
OUT = chip8

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
