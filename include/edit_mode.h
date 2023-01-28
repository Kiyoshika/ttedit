#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>
#include <stdbool.h>

// forward declarations
struct screen_buffer_t;
struct cursor_t;

// write a key press to the buffer while
// user is in EDIT mode
void edit_write_key(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const int key_pressed);

// shift the buffer down from a given cursor row index
void edit_shift_buffer_down(
		struct screen_buffer_t* const screen,
		const size_t row_idx);

// shift the buffer up from a given cursor row index
void edit_shift_buffer_up(
		struct screen_buffer_t* const screen,
		const size_t row_idx);

// insert new line into a buffer with the option to copy
// the buffer from cursor to end of line and move it to the
// next line
void edit_insert_new_line(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const bool copy_buffer);

// delete current line from buffer and shift buffer up
void edit_delete_current_line(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

// take selected text and copy contents into a buffer
void edit_copy_buffer(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

// insert copy buffer into current cursor position
void edit_paste_buffer(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor);

#endif
