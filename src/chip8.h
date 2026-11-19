#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 4096
#define ROM_START_ADDRESS 0x200
#define FONT_START_ADDRESS 0x000
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef struct {
    uint16_t reg_i;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t memory[MEMORY_SIZE];
    uint8_t registers[16];
    uint8_t reg_dt;
    uint8_t reg_st;
    uint8_t sp;
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    uint8_t keypad[16];
} chip8_t;

void chip8_init(chip8_t* chip8);
bool chip8_load_rom(chip8_t* chip8, const char* rom_path);
void chip8_cycle(chip8_t* chip8);
void chip8_tick_timers(chip8_t* chip8, void (*on_sound_active)(void));
const uint8_t* chip8_get_display(const chip8_t* chip8);

#endif