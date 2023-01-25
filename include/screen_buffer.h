#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

#include <ncurses.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b

#define LINE_BUFF_SIZE 256
#define DOUBLE_LINE_BUFF_SIZE LINE_BUFF_SIZE * 2

// forward declarations
struct window_t;
struct cursor_t;

struct screen_buffer_t
{
	// all lines in the buffer (e.g., entire source file)
	char (*lines)[LINE_BUFF_SIZE];
	size_t total_lines;
	// the largest (occupied) line meaning the latest line with content.
	// this is used for locking the cursor to prevent moving past our buffer
	size_t max_occupied_line;
	// current line index on the screen buffer (values between start_idx & end_idx)
	size_t current_line;
	// start index of which lines to display
	size_t start_idx;
	// end index of which lines to display (NOTE: end_idx - start_idx = max_rows)
	size_t end_idx;
	// max rows visible on screen
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
		const struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

// draw the buffer on the current cursor line position
// to avoid redrawing the entire screen buffer.
void screen_draw_line(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor);

void screen_scroll_up(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

void screen_scroll_down(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

// free the memory allocated by the line buffer
void screen_free(
		struct screen_buffer_t* screen);

// read contents of file to populate screen buffer
// (used when first opening ttedit)
bool screen_read_file(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const char* filename);

// write the contents (lines) to the output file
bool screen_write_to_file(
		const struct screen_buffer_t* const screen,
		const char* filename);

#endif
