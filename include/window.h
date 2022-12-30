#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <ncurses.h>

struct window_t
{
	uint32_t rows;
	uint32_t columns;
};

// Pass a window_t struct by address
// to initialise its contents (by default
// it will grab the max rows/columns of the current
// terminal window)
void window_init(
		struct window_t* const window);

// Pass a window_t struct by address
// and fetch the current max row/column sizes
// for the terminal window.
void window_refresh_size(
		struct window_t* const window);

#endif
