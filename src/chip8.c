#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

static void load_fonts(chip8_t* chip8);
static void op_drw(chip8_t* chip8, uint8_t x, uint8_t y, uint8_t n);
static void op_ld_vx_k(chip8_t* chip8, uint8_t x);

void chip8_init(chip8_t* chip8) {
    // Clean all the struct
    memset(chip8, 0, sizeof(chip8_t));

    chip8->pc = ROM_START_ADDRESS;

    srand((unsigned int)time(NULL));

    load_fonts(chip8);
}

static void load_fonts(chip8_t* chip8) {
    static const uint8_t font_set[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(&chip8->memory[FONT_START_ADDRESS], font_set, sizeof(font_set));
}

bool chip8_load_rom(chip8_t* chip8, const char* rom_path) {
    FILE* rom_file;
    long rom_size;
    size_t bytes_read;

    rom_file = fopen(rom_path, "rb");

    if (rom_file == NULL) {
        return false;
    }

    // Go to the end and count all the bytes, then go back to start
    fseek(rom_file, 0, SEEK_END);
    rom_size = ftell(rom_file);
    rewind(rom_file);

    // ROM too big (or too small)!
    if (rom_size <= 0 || rom_size > (MEMORY_SIZE - ROM_START_ADDRESS)) {
        fclose(rom_file);
        return false;
    }

    bytes_read = fread(&chip8->memory[ROM_START_ADDRESS], 1, rom_size, rom_file);

    fclose(rom_file);

    return bytes_read == (size_t)rom_size;
}

void chip8_cycle(chip8_t* chip8) {
    uint16_t opcode;
    uint8_t x;   // Lower 4 bits of high byte 
    uint8_t y;   // Upper 4 bits of low byte
    uint8_t n;   // Lower 4 bits of the instruction
    uint8_t kk;  // Lowest 8 bits of the instruction
    uint16_t nnn; // Lowest 12 bits of the instruction
    uint16_t result; // For storing temp results
    
    // Fetch
    opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    
    // We prepare for the next instruction BEFORE executing the current one
    chip8->pc += 2;

    x = (opcode >> 8) & 0x000F;
    y = (opcode >> 4) & 0x000F;
    n = opcode & 0x000F;
    kk = opcode & 0x00FF;
    nnn = opcode & 0x0FFF;

    // Decode
    // (opcode & 0xF000) takes the most important byte which should tell us the instruction
    switch(opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                // CLS: Clear screen
                case 0x00E0: 
                    memset(chip8->display, 0, sizeof(chip8->display));
                    break;

                // RET: Return to subroutine
                case 0x00EE:
                    chip8->pc = chip8->stack[chip8->sp];
                    chip8->sp--;
                    break;
            }
            break;

        // JP nnn: Jump to address
        case 0x1000:
            chip8->pc = nnn;
            break;
        
        // CALL addr: Call subroutine at nnn
        case 0x2000:
            chip8->sp++;
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->pc = nnn;
            break;
        
        // SE Vx, byte: Skip next instruction if Vx = kk
        case 0x3000:
            if (chip8->registers[x] == kk) {
                chip8->pc += 2;
            }
            break;
        
        // SNE Vx, byte: Skip next instruction if Vx != kk
        case 0x4000:
            if (chip8->registers[x] != kk) {
                chip8->pc += 2;
            }
            break;
        
        // SE Vx, Vy: Skip next instruction if Vx = Vy
        case 0x5000:
            if (chip8->registers[x] == chip8->registers[y]) {
                chip8->pc += 2;
            }
            break;
        
        // LD Vx, byte: Set Vx = kk
        case 0x6000:
            chip8->registers[x] = kk;
            break;
        
        // ADD Vx, byte: Set Vx = Vx + kk
        case 0x7000:
            chip8->registers[x] += kk;
            break;
        
        case 0x8000:
            switch (n) {
                // LD Vx, Vy: Set Vx = Vy
                case 0:
                    chip8->registers[x] = chip8->registers[y];
                    break;
                
                // OR Vx, Vy: Set Vx = Vx OR Vy
                case 1:
                    chip8->registers[x] = chip8->registers[x] | chip8->registers[y];
                    break;

                // AND Vx, Vy: Set Vx = Vx AND Vy
                case 2:
                    chip8->registers[x] = chip8->registers[x] & chip8->registers[y];
                    break;
                
                // XOR Vx, Vy: Set Vx = Vx XOR Vy
                case 3:
                    chip8->registers[x] = chip8->registers[x] ^ chip8->registers[y];
                    break;
                
                // ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry
                case 4:
                    result = chip8->registers[x] + chip8->registers[y];

                    chip8->registers[0xF] = result > 0xFF ? 1 : 0;
                    chip8->registers[x] = result;
                    break;
                
                // SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
                case 5:
                    chip8->registers[0xF] = chip8->registers[x] > chip8->registers[y] ? 1 : 0;
                    chip8->registers[x] -= chip8->registers[y];
                    break;
                
                // SHR Vx {, Vy}: Set Vx = Vx SHR 1.
                case 6:
                    chip8->registers[0xF] = chip8->registers[x] & 0x01;
                    chip8->registers[x] /= 2;
                    break;
                
                // SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
                case 7:
                    chip8->registers[0xF] = chip8->registers[y] > chip8->registers[x] ? 1 : 0;
                    chip8->registers[x] = chip8->registers[y] - chip8->registers[x];
                    break;
                
                // SHL Vx {, Vy}: Set Vx = Vx SHL 1.
                case 0xE:
                    chip8->registers[0xF] = (chip8->registers[x] & 0x80) ? 1 : 0;
                    chip8->registers[x] *= 2;
                    break;
            }
            break;
        
        // SNE Vx, Vy: Skip next instruction if Vx != Vy.
        case 0x9000:
            if (chip8->registers[x] != chip8->registers[y]) {
                chip8->pc += 2;
            }
            break;
        
        // LD I, addr: Set I = nnn
        case 0xA000:
            chip8->reg_i = nnn;
            break;
        
        // JP V0, addr: Jump to location nnn + V0
        case 0xB000:
            chip8->pc = nnn + chip8->registers[0];
            break;
        
        // RND Vx, byte: Set Vx = random byte AND kk
        case 0xC000:
            chip8->registers[x] = (rand() % 256) & kk;
            break;
        
        // DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        case 0xD000:
            op_drw(chip8, x, y, n);
            break;
        
        case 0xE000:
            switch (n) {
                // SKP Vx: Skip next instruction if key with the value of Vx is pressed.
                case 0xE:
                    if (chip8->keypad[chip8->registers[x] & 0x0F] != 0) { // & 0x0F prevents overflow
                        chip8->pc += 2;
                    }
                    break;
                
                // SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
                case 1:
                    if (chip8->keypad[chip8->registers[x] & 0x0F] == 0) { // & 0x0F prevents overflow
                        chip8->pc += 2;
                    }
                    break;
            }
            break;
        
        case 0xF000:
            switch (kk) {
                // LD Vx, DT: Set Vx = delay timer value.
                case 0x07:
                    chip8->registers[x] = chip8->reg_dt;
                    break;
                
                // LD Vx, K: Wait for a key press, store the value of the key in Vx.
                case 0x0A:
                    op_ld_vx_k(chip8, x);
                    break;
                
                // LD DT, Vx: Set delay timer = Vx.
                case 0x15:
                    chip8->reg_dt = chip8->registers[x];
                    break;

                // LD ST, Vx: Set sound timer = Vx.
                case 0x18:
                    chip8->reg_st = chip8->registers[x];
                    break;

                // ADD I, Vx: Set I = I + Vx.
                case 0x1E:
                    chip8->reg_i += chip8->registers[x];
                    break;

                // LD F, V: Set I = location of sprite for digit Vx.
                case 0x29:
                    // We multiply by 5 because each hexadecimal sprite has 5 bytes
                    chip8->reg_i = FONT_START_ADDRESS + ((chip8->registers[x] & 0x0F) * 5);
                    break;
                
                // LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
                case 0x33:
                    chip8->memory[chip8->reg_i] = chip8->registers[x] / 100;
                    chip8->memory[chip8->reg_i + 1] = (chip8->registers[x] / 10) % 10;
                    chip8->memory[chip8->reg_i + 2] = chip8->registers[x] % 10;
                    break;
                
                // LD [I], Vx: Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    memcpy(&chip8->memory[chip8->reg_i], chip8->registers, sizeof(chip8->registers[0]) * (x + 1));
                    break;
                
                // LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
                case 0x65:
                    memcpy(chip8->registers, &chip8->memory[chip8->reg_i], sizeof(chip8->registers[0]) * (x + 1));
                    break;
            }
            break;
    }
}

static void op_drw(chip8_t* chip8, uint8_t x, uint8_t y, uint8_t n) {    
    uint8_t byte;
    const uint8_t start_x = chip8->registers[x] % DISPLAY_WIDTH; // Wrap around the screen
    const uint8_t start_y = chip8->registers[y] % DISPLAY_HEIGHT;
    uint8_t pixel_x;
    uint8_t pixel_y;
    uint16_t index;

    chip8->registers[0xF] = 0;

    // For each byte (starting from reg_i up to reg_i + n times)
    for (int row = 0; row < n; row++) {
        // Get the next byte
        byte = chip8->memory[chip8->reg_i + row];

        // For each bit in the byte (pixel)
        for (int pixel = 0; pixel < 8; pixel++) {
            // If the pixel was already on
            if ((byte & (0x80 >> pixel)) != 0) {
                pixel_x = start_x + pixel; // Get the column (x)
                pixel_y = start_y + row;   // Get the row (y)

                // Pixel out of screen. Ignoring...
                if (pixel_x >= DISPLAY_WIDTH || pixel_y >= DISPLAY_HEIGHT) {
                    continue;
                }

                // Convert from 2D array to 1D
                index = (pixel_y * DISPLAY_WIDTH) + pixel_x;

                // Collision!
                if (chip8->display[index] == 1) {
                    chip8->registers[0xF] = 1;
                }

                chip8->display[index] ^= 1; // if 0 then 1 (on) | if 1 then 0 (off)
            }
        }
    }
}

static void op_ld_vx_k(chip8_t* chip8, uint8_t x) {
    bool key_pressed = false;

    for (int i = 0; i < 16; i++) {
        if (chip8->keypad[i] != 0) {
            chip8->registers[x] = i;
            key_pressed = true;
        }
    }

    // Key not detected! Executing this instruction again.
    if (!key_pressed) {
        chip8->pc -= 2;
    }   
}

void chip8_tick_timers(chip8_t* chip8) {
    if (chip8->reg_dt > 0) {
        chip8->reg_dt--;
    }

    if (chip8->reg_st > 0) {
        chip8->reg_st--;
    }
}

const uint8_t* chip8_get_display(const chip8_t* chip8) {
    return chip8->display;
}

bool chip8_is_sound_active(const chip8_t* chip8) {
    return chip8->reg_st > 0;
}