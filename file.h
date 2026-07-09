#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>

long get_file_size(const char *filename, struct stat *st);
void read_to_memory(uint8_t *memory, const char *filename, long filesize);
//bool is_valid_ext(const char *filename);

#endif
