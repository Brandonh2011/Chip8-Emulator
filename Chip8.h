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
	uint16_t stack[12];
	uint8_t stack_ptr;

	struct inst
	{
		uint8_t N;
		uint8_t NN;
		uint16_t NNN;
		uint8_t x;
		uint8_t y;
	} inst;

} Chip8;

#endif
