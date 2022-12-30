#ifndef LINE_H
#define LINE_H

#include <stddef.h>
#include <string.h>

#define LINE_BUFF_SIZE 101

struct line_t
{
	size_t number;
	size_t buffer_idx;
	char buffer[LINE_BUFF_SIZE];
};

// pass a line_t struct by address
// to initialise its contents
void line_create(
		struct line_t* const line);

// append a character to the current line buffer.
// any characters past buffer length are ignored.
void line_append(
		struct line_t* const line,
		const char ch);

// TODO: backspace, clear, delete, insert, etc.

#endif
