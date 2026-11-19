# CHIP-8 Emulator

A CHIP-8 interpreter written in C11. Currently runs in the terminal for testing and debugging core instructions, with SDL2 support planned for graphics, input, and sound.

## Features

- Full original 35-opcode instruction set
- 4KB memory, 16 8-bit registers, stack, delay and sound timers
- 64x32 monochrome display rendered in ASCII
- Decoupled core logic ready for a graphical frontend

## Build

Requires gcc and make:

    make

## Usage

Pass the path to a .ch8 ROM file:

    ./chip8 roms/ibm.ch8

## Roadmap

- [x] Core CPU and opcode decoding
- [x] Terminal display output
- [ ] SDL2 window and pixel rendering
- [ ] Hex keypad mapping
- [ ] Audio output for the sound timer
