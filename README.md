# CHIP-8 Emulator

A CHIP-8 interpreter written in C11 using SDL2 for graphics, input, and audio.

## Features

- Full original 35-opcode instruction set
- 4KB memory, 16 8-bit registers, stack, delay and sound timers
- 64x32 monochrome display scaled via SDL2 streaming textures
- 16-key hexadecimal keypad mapped to QWERTY
- 440Hz square wave buzzer for the sound timer

## Requirements

- `gcc`
- `make`
- `libsdl2-dev`

On Debian/Ubuntu:

    sudo apt install build-essential libsdl2-dev

## Build

    make

## Usage

Pass the path to a `.ch8` ROM file:

    ./chip8 roms/pong.ch8

Press `ESC` or close the window to quit.

## Controls

The original 4x4 hexadecimal keypad is mapped to the left side of a QWERTY keyboard:

    CHIP-8 Keypad       PC Keyboard
    +---+---+---+---+   +---+---+---+---+
    | 1 | 2 | 3 | C |   | 1 | 2 | 3 | 4 |
    +---+---+---+---+   +---+---+---+---+
    | 4 | 5 | 6 | D |   | Q | W | E | R |
    +---+---+---+---+   +---+---+---+---+
    | 7 | 8 | 9 | E |   | A | S | D | F |
    +---+---+---+---+   +---+---+---+---+
    | A | 0 | B | F |   | Z | X | C | V |
    +---+---+---+---+   +---+---+---+---+