#include "line.h"

void line_create(
		struct line_t* const line)
{
	line->number = 0;
	line->buffer_idx = 0;
	memset(line->buffer, 0, LINE_BUFF_SIZE);
}

void line_append(
		struct line_t* const line,
		const char ch)
{
	if (line->buffer_idx < (size_t)LINE_BUFF_SIZE - 1)
		line->buffer[line->buffer_idx++] = ch;
}
