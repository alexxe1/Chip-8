#include <stdio.h>
#include "chip8.h"
#include "platform.h"

#define WINDOW_TITLE "Chip-8 Emulator"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int main(int argc, char *argv[]) {
    chip8_t chip8;
    platform_t platform;
    const uint32_t frame_delay = 1000 / 60;
    uint32_t frame_start;
    uint32_t frame_time;
    bool running = true;

    if (argc < 2) {
        printf("Usage: %s <rom_path>\n", argv[0]);
        return 1;
    }

    chip8_init(&chip8);

    if (!chip8_load_rom(&chip8, argv[1])) {
        printf("Couldn't load ROM at %s\n", argv[1]);
        return 1;
    }
    
    if (!platform_init(&platform, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        printf("Couldn't initialize SDL2! Error code: %s\n", SDL_GetError());
        return 1;
    }
    
    while (running) {
        frame_start = SDL_GetTicks();

        running = platform_process_input(chip8.keypad);

        for (int i = 0; i < 10; i++) {
            chip8_cycle(&chip8);
        }

        chip8_tick_timers(&chip8, NULL);
        platform_update(&platform, chip8_get_display(&chip8));

        frame_time = SDL_GetTicks() - frame_start;

        if (frame_time < frame_delay) {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    platform_destroy(&platform);

    return 0;
}