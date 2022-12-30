#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>

struct cursor_t
{
	uint32_t row;
	uint32_t column;
};

// pass a cursor_t by address
// to initialise its contents to 0.
void cursor_init(
		struct cursor_t* const cursor);
#endif
