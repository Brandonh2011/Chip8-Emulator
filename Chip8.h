#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct
{
	uint8_t memory[4096];
	uint8_t screen[32][64];

	uint8_t V[16];

	uint16_t PC;
	uint16_t I;
} Chip8;

#endif
