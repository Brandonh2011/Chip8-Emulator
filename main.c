#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "file.h"
#include "Chip8.h"

#define SCALE 16

void drawScreen(SDL_Surface *surface, Chip8 *chip8)
{
	Uint32 color;
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (chip8->screen[y][x] == 0)
			{
				color = SDL_MapRGB(surface->format, 0, 0, 0); // BLACK
			}
			else
			{
				color = SDL_MapRGB(surface->format, 255, 255, 255); // WHITE
			}
			SDL_Rect rect = {x*SCALE, y*SCALE, SCALE, SCALE};
			SDL_FillRect(surface, &rect, color);
		}
	}
}

void drawSprite(Chip8 *chip8, uint8_t xPos, uint8_t yPos, uint8_t n)
{
	for (int row = 0; row < n; row++)
	{
		uint8_t sb = chip8->memory[chip8->I + row];

		for (int col = 0; col < 8; col++)
		{
			if ((sb & (0x80 >> col)) != 0)
			{
				uint8_t sx = (xPos + col) % 64;
				uint8_t sy = (yPos + row) % 32;
				uint8_t *pixel = &chip8->screen[sy][sx];
				if (*pixel == 1)
					chip8->V[0xF] = 1;
				*pixel ^= 1;
			}
		}
	}
}

void emulateInstruction(Chip8 *chip8)
{
	uint8_t byte1 = chip8->memory[chip8->PC];
	uint8_t byte2 = chip8->memory[chip8->PC+1];
	uint16_t opcode = (byte1 << 8) | byte2;
	chip8->inst.NNN = opcode & 0x0FFF;
	chip8->inst.NN = opcode & 0x00FF;
	chip8->inst.N = opcode & 0x0F;
	chip8->inst.x = (opcode & 0x0F00) >> 8;
	chip8->inst.y = (opcode & 0x00F0) >> 4;
	chip8->PC += 2;
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode) {
		case 0x00E0:
			// CLS
			memset(chip8->screen, 0, sizeof(chip8->screen));
			break;
		case 0x00EE:
			// RET
			if (chip8->stack_ptr == 0)
			{
				printf("Stack underflow.\n");
				exit(1);
			}
			chip8->stack_ptr--;
			chip8->PC = chip8->stack[chip8->stack_ptr];
			break;
		}
		break;
	case 0x1000:
		// JMP
		chip8->PC = chip8->inst.NNN;
		break;
	case 0x2000:
		// CALL
		if (chip8->stack_ptr >= 12)
		{
			printf("Stack overflow.\n");
			exit(1);
		}
		chip8->stack[chip8->stack_ptr] = chip8->PC; 
		chip8->stack_ptr++;
		chip8->PC = chip8->inst.NNN;
		break;
	case 0x3000:
		if (chip8->V[chip8->inst.x] == chip8->inst.NN)
			chip8->PC += 2;
		break;
	case 0x4000:
		if (chip8->V[chip8->inst.x] != chip8->inst.NN)
			chip8->PC += 2;
		break;
	case 0x5000:
		if (chip8->V[chip8->inst.x] == chip8->V[chip8->inst.y])
			chip8->PC += 2;
		break;
	case 0x6000:
		chip8->V[chip8->inst.x] = chip8->inst.NN;
		break;
	case 0x7000:
		chip8->V[chip8->inst.x] += chip8->inst.NN;
		break;
	case 0x9000:
		if (chip8->V[chip8->inst.x] != chip8->V[chip8->inst.y])
			chip8->PC += 2;
		break;
	case 0xA000:
		chip8->I = chip8->inst.NNN;
		break;
	case 0xD000:
	{
		uint8_t xPos = chip8->V[chip8->inst.x];
		uint8_t yPos = chip8->V[chip8->inst.y];
		chip8->V[0xF] = 0;
		drawSprite(chip8, xPos, yPos, chip8->inst.N);
	}
		break;
	default:
		printf("Opcode: 0x%4X not yet implemented.\n", opcode);
	}
	if (chip8->PC >=4096)
		exit(0);
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Missing ROM file.\n");
		printf("Usage: %s <ch8 ROM file>", argv[0]);
		return 1;
	}
	const char* rom = argv[1];
	struct stat st;
	long filesize = get_file_size(rom, &st);
	if (filesize == 0 || filesize > 4096 - 0x200)
	{
		printf("ROM file is too big or corrupted.\n");
		return 1;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Window *win = 
		SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64*SCALE, 32*SCALE, 0);
	if (!win)
	{
		printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Surface *sur = SDL_GetWindowSurface(win);
	if (!sur)
	{
		printf("SDL_GetWindowSurface failed: %s\n", SDL_GetError());
		return 1;
	}

	Chip8 chip8 = {0};
	read_to_memory(chip8.memory, rom, filesize);
	chip8.PC = 0x200;
	chip8.stack_ptr = 0;
	bool running = true;
	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		emulateInstruction(&chip8);
		drawScreen(sur, &chip8);
		SDL_UpdateWindowSurface(win);
		SDL_Delay(2);
	}
	SDL_FreeSurface(sur);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
