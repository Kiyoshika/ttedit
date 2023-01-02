#include "cursor.h"
#include "screen_buffer.h"

void cursor_init(
		struct cursor_t* const cursor)
{
	cursor->row = 0;
	cursor->column = 0;
	cursor->line_num_size = 2;
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
		struct cursor_t* const cursor)
{
	if (cursor->column > 0)
		cursor->column--;

	move(cursor->row, cursor->column + cursor->line_num_size + 1);
	refresh();
}

void cursor_move_right(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen)
{
	if (cursor->column < strlen(screen->lines[cursor->row]))
		cursor->column++;

	move(cursor->row, cursor->column + cursor->line_num_size + 1);
	refresh();
}

void cursor_prepend_line(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen)
{
	for (size_t i = 0; i < strlen(screen->lines[cursor->row]); ++i)
	{
		if (screen->lines[cursor->row][i] != ' ')
		{
			cursor->column = i;
			move(cursor->row, cursor->column);
			refresh();
			break;
		}
	}
}

void cursor_append_line(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen)
{
	// clamp the cursor position to the max allowed length of the line buffer
	size_t position = strlen(screen->lines[cursor->row]) > LINE_BUFF_SIZE - 1 ? LINE_BUFF_SIZE - 1 : strlen(screen->lines[cursor->row]);
	cursor->column = position;
	move(cursor->row, cursor->column);
	refresh();
}

// move cursor to the bottom of the visual buffer (no scrolling)
void cursor_jump_visual_bottom(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	size_t position = screen->max_occupied_line - 1 < screen->max_rows ? screen->max_occupied_line : screen->max_rows - 1;
	position -= 1;
	size_t difference = position - screen->current_line;
	cursor->row += difference;
	screen->current_line = position;
	move(cursor->row, cursor->column);
	screen_draw(screen, cursor);
}

// move cursor to the bottom of the entire buffer (scrolling if needed).
// we have to take some extra care in case we have not allocated as many lines
// as the screen length
void cursor_jump_bottom(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	cursor->row = screen->max_occupied_line - 1;
	if (cursor->row > screen->max_rows)
	{
		cursor->row += 2;
		screen->end_idx = cursor->row;
		screen->start_idx = screen->end_idx - screen->max_rows;
	}
	screen->current_line = cursor->row > screen->max_rows ? screen->max_rows : cursor->row;
	move(screen->current_line, cursor->column);
	refresh();
	screen_draw(screen, cursor);
}

void cursor_jump_visual_top(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	cursor->row -= screen->current_line;
	screen->current_line = 0;
	move(cursor->row, cursor->column);
	screen_draw(screen, cursor);
}

void cursor_jump_top(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	cursor->row = 0;
	screen->current_line = 0;
	screen->start_idx = 0;
	screen->end_idx = screen->max_rows;
	move(screen->current_line, cursor->column);
	refresh();
	screen_draw(screen, cursor);
}
