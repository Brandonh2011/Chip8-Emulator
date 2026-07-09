#include "file.h"
#include <stdio.h>
#include <stdlib.h>

long get_file_size(const char *filename, struct stat *st)
{
	if (stat(filename, st) != 0)
	{
		perror("Error getting file size");
		return (long)0;
	}
	return (long)st->st_size;
}

void read_to_memory(uint8_t *memory, const char *filename, long filesize)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		perror("Error");
		exit(1);
	}
	fread(&memory[0x200], 1, filesize, fp);
	fclose(fp);
}
