#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>

// forward declarations
struct screen_buffer_t;
struct cursor_t;

// write a key press to the buffer while
// user is in EDIT mode
void edit_write_key(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const int key_pressed);
#endif
