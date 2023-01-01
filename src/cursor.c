#include "cursor.h"
#include "screen_buffer.h"

void cursor_init(
		struct cursor_t* const cursor)
{
	cursor->row = 0;
	cursor->column = 0;
}

void cursor_move_down(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	// prevent user from accessing unallocated space
	if (cursor->row + 1 >= screen->max_occupied_line)
		return;

	cursor->row++;
	screen->current_line++;

	if (screen->current_line == screen->max_rows - 1)
	{
		screen->current_line--;
		screen_scroll_down(screen, cursor);
	}

	if (cursor->column > strlen(screen->lines[cursor->row]))
		cursor->column = strlen(screen->lines[cursor->row]);

	move(screen->current_line, cursor->column);
	refresh();
}

void cursor_move_up(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	if (cursor->row > 0)
		cursor->row--;

	if (screen->current_line > 0)
		screen->current_line--;

	if (screen->current_line == 0)
		screen_scroll_up(screen, cursor);

	if (cursor->column > strlen(screen->lines[cursor->row]))
		cursor->column = strlen(screen->lines[cursor->row]);

	move(screen->current_line, cursor->column);
	refresh();
}

void cursor_move_left(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	if (cursor->column > 0)
		cursor->column--;

	move(cursor->row, cursor->column);
	refresh();
}

void cursor_move_right(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	if (cursor->column < strlen(screen->lines[cursor->row]))
		cursor->column++;

	move(cursor->row, cursor->column);
	refresh();
}
