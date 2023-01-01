#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include <ncurses.h>
#include <string.h>

// forward declaration
struct screen_buffer_t;

struct cursor_t
{
	uint32_t row;
	uint32_t column;
};

// pass a cursor_t by address
// to initialise its contents to 0.
void cursor_init(
		struct cursor_t* const cursor);

void cursor_move_left(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

void cursor_move_right(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

void cursor_move_up(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

void cursor_move_down(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);
#endif
