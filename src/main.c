#include <stdio.h>
#include <time.h>
#include "chip8.h"

#define CYCLES_PER_FRAME 10 // 10 * 60 = 600 Hz de CPU

static void render_console(const chip8_t* chip8);

int main(int argc, char *argv[]) {
    chip8_t chip8;
    bool rom_loaded = false;
    struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = 16000000L // 16.000.000 nanosegundos = 16 milisegundos
    };

    if (argc < 2) {
        printf("Usage: %s <rom_path>\n", argv[0]);
        return -1;
    }
    
    puts("Initializing Chip-8 emulator!");

    chip8_init(&chip8);

    printf("Loading rom from: %s...\n", argv[1]);

    rom_loaded = chip8_load_rom(&chip8, argv[1]);

    printf("Loaded: %d\n", rom_loaded);

    while (true) {
        for (int i = 0; i < CYCLES_PER_FRAME; i++) {
            chip8_cycle(&chip8);
        }

        chip8_tick_timers(&chip8, NULL);
        render_console(&chip8);

        nanosleep(&ts, NULL);
    }

    return 0;
}

static void render_console(const chip8_t* chip8) {
    const uint8_t* display = chip8_get_display(chip8);
    uint8_t pixel;

    // Mueve el cursor arriba a la izquierda para sobreescribir el frame anterior sin parpadeo
    printf("\033[H");

    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            pixel = display[(y * DISPLAY_WIDTH) + x];

            printf("%c", pixel ? '#' : ' ');
        }
        printf("\n");
    }
}