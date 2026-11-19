#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 32

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixel_buffer[TEXTURE_WIDTH * TEXTURE_HEIGHT];
} platform_t;

bool platform_init(platform_t* platform, const char* title, int window_width, int window_height);
void platform_destroy(platform_t* platform);
void platform_update(platform_t* platform, const uint8_t* display);
bool platform_process_input(uint8_t* keypad);

#endif