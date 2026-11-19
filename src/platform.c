#include <string.h>
#include "platform.h"

static int map_key(SDL_Keycode key);
static void audio_callback(void* userdata, uint8_t* stream, int len);

bool platform_init(platform_t* platform, const char* title, int window_width, int window_height) {
    SDL_AudioSpec desired_spec = {
        .freq = 44100,
        .format = AUDIO_S16SYS, // 16 bits signed int
        .channels = 1, // Mono
        .samples = 512,
        .callback = audio_callback
    };
    
    memset(platform, 0, sizeof(platform_t));
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return false;
    }

    platform->window = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (platform->window == NULL) {
        SDL_Quit();
        return false;
    }

    platform->renderer = SDL_CreateRenderer(platform->window, -1, SDL_RENDERER_ACCELERATED);

    if (platform->renderer == NULL) {
        SDL_DestroyWindow(platform->window);
        SDL_Quit();
        return false;
    }

    platform->texture = SDL_CreateTexture(
        platform->renderer, 
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        TEXTURE_WIDTH,
        TEXTURE_HEIGHT
        );
    
    if (platform->texture == NULL) {
        SDL_DestroyRenderer(platform->renderer);
        SDL_DestroyWindow(platform->window);
        SDL_Quit();
        return false;
    }

    platform->audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, NULL, 0);

    if (platform->audio_device == 0) {
        SDL_DestroyTexture(platform->texture);
        SDL_DestroyRenderer(platform->renderer);
        SDL_DestroyWindow(platform->window);
        SDL_Quit();
        return false;
    }

    return true;
}

void platform_destroy(platform_t* platform) {
    SDL_DestroyTexture(platform->texture);
    SDL_DestroyRenderer(platform->renderer);
    SDL_DestroyWindow(platform->window);
    SDL_CloseAudioDevice(platform->audio_device);
    SDL_Quit();
}

void platform_update(platform_t* platform, const uint8_t* display) {
    for (int i = 0; i < TEXTURE_WIDTH * TEXTURE_HEIGHT; i++) {
        platform->pixel_buffer[i] = display[i] != 0 ? 0xA25112FF : 0x000000FF;
    }

    SDL_UpdateTexture(platform->texture, NULL, platform->pixel_buffer, TEXTURE_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(platform->renderer);
    SDL_RenderCopy(platform->renderer, platform->texture, NULL, NULL);
    SDL_RenderPresent(platform->renderer);
}

bool platform_process_input(uint8_t* keypad) {
    SDL_Event event;
    int key;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return false;
            }

            key = map_key(event.key.keysym.sym);

            if (key != -1) {
                keypad[key] = event.type == SDL_KEYDOWN ? 1 : 0;
            }
        }
    }

    return true;
}

void platform_set_sound(platform_t* platform, bool active) {
    SDL_PauseAudioDevice(platform->audio_device, active ? 0 : 1);
}

static int map_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_x: return 0x0;
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;
        case SDLK_z: return 0xA;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;
        default: return -1;
    }
}

static void audio_callback(void* userdata, uint8_t* stream, int len) {
    (void)userdata;
    int16_t* buffer = (int16_t*)stream;
    int samples = len / sizeof(int16_t);
    static uint32_t sample_index = 0;

    // 44,100 Hz / 440 Hz = ~100 samples per complete wave cycle (50 above, 50 below).
    const int period = 100;
    const int16_t amplitude = 3000; // Moderate volume (the maximum value of int16_t is 32767)

    for (int i = 0; i < samples; i++) {
        buffer[i] = ((sample_index / (period / 2)) % 2 == 0) ? amplitude : -amplitude;
        sample_index++;
    }
}