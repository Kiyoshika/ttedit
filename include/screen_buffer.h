#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

#include <ncurses.h>
#include <stdint.h>

// forward declarations
struct line_array_t;
struct window_t;
struct cursor_t;

struct screen_buffer_t
{
	struct line_array_t* lines;
	size_t current_line;
	size_t start_idx;
	size_t end_idx;
	size_t max_rows;
};

// pass a screen_buffer_t by address to initialise
// its contents.
int8_t screen_init(
		struct screen_buffer_t* const screen,
		const struct window_t* const window);

// draw the screen buffer by writing the lines
// from start_idx to end_idx
void screen_draw(
		const struct screen_buffer_t* const screen);

// draw the buffer on the current cursor line position
// to avoid redrawing the entire screen buffer.
void screen_draw_line(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor);

#endif
