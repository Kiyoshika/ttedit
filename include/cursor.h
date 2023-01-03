#ifndef CURSOR_H
#define CURSOR_H

#include <stdint.h>
#include <ncurses.h>
#include <string.h>

// forward declaration
struct screen_buffer_t;

struct cursor_t
{
	// current row in total buffer we're pointing at
	uint32_t row;
	// current column (character) in current line we're pointing at
	uint32_t column;
	// total space the line numbers take up (will need to subtract this while writing to buffer)
	uint16_t line_num_size;
};

// pass a cursor_t by address
// to initialise its contents to 0.
void cursor_init(
		struct cursor_t* const cursor);

void cursor_move_left(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen);

void cursor_move_right(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen);

void cursor_move_up(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

void cursor_move_down(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

// move the cursor to the first non-space character in buffer
void cursor_prepend_line(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen);

// move the cursor to the end of the line
void cursor_append_line(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen);

// move cursor to the bottom of the visual buffer (no scrolling)
void cursor_jump_visual_bottom(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

// move cursor to the bottom of the entire buffer (scrolling if needed)
void cursor_jump_bottom(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

// move cursor to the top of the visual buffer (no scrolling)
void cursor_jump_visual_top(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

// move cursor to the top of the entire buffer (scrolling if needed)
void cursor_jump_top(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen);

// jump cursor to specific line number
void cursor_jump_line(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen,
		const uint32_t line_num);

#endif
