#include "window.h"

void window_init(
		struct window_t* const window)
{
	window->rows = 0;
	window->columns = 0;
	window_refresh_size(window);
}

void window_refresh_size(
		struct window_t* const window)
{
	getmaxyx(stdscr, window->rows, window->columns);
}
