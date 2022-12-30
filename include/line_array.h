#ifndef LINE_ARRAY_H
#define LINE_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// forward declaration
struct line_t;

struct line_array_t
{
	struct line_t* line;
	size_t current_line;
	size_t capacity;
};

// allocate and return line_array_t pointer
// with desired capacity. returns NULL on failure.
struct line_array_t* line_array_create(
		size_t capacity);

// copy contents of a line_t into line_array.
// returns 0 on success, -1 otherwise.
int8_t line_array_add(
		struct line_array_t* const line_array,
		const struct line_t* const line);

// free memory occupied by line_array
void line_array_free(
		struct line_array_t* line_array);
#endif
