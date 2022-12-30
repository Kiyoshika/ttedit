#include "edit_mode.h"
#include "screen_buffer.h"
#include "line.h"
#include "cursor.h"
#include "line_array.h"

/*static void _scroll_window_up(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	screen->start_idx--;
	screen->end_idx--;
	screen_draw(screen);
}*/

static void _scroll_window_down(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	screen->start_idx++;
	screen->end_idx++;
	screen_draw(screen);
}

void edit_write_key(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const int key_pressed)
{
	if (key_pressed == '\n' || key_pressed == KEY_ENTER)
	{
		if (screen->current_line < screen->max_rows - 1)
			screen->current_line++;
		cursor->row++;

		if (screen->current_line >= screen->end_idx - 1)
		{
			struct line_t line;
			line_create(&line);
			line_array_add(screen->lines, &line);

			screen->current_line--;
			cursor->row--;
			_scroll_window_down(screen, cursor);
		}

		cursor->column = 0;
		move(cursor->row, cursor->column);
	}
	else if (key_pressed == KEY_BACKSPACE || key_pressed == 127)
	{
		// remove current character and shift all elements from the right over
		// to the left
		char* buffer = screen->lines->line[cursor->row].buffer;

		// if current line buffer is empty, wrap up to previous row
		if (strlen(buffer) == 0)
		{
			if (cursor->row > 0)
				cursor->row--;
			if (screen->current_line > 0)
				screen->current_line--;
			cursor->column = strlen(screen->lines->line[cursor->row].buffer);
			return;
		}

		size_t i = cursor->column - 1;
		for (; i < strlen(buffer) - 1; ++i)
			buffer[i] = buffer[i+1];
		buffer[i] = '\0';

		cursor->column--;

		screen_draw_line(screen, cursor);
	}
	else
	{
		// write key to buffer and wrap cursor if necessary
		screen->lines->line[cursor->row].buffer[cursor->column++] = key_pressed;
		screen_draw_line(screen, cursor);
		if (cursor->column == LINE_BUFF_SIZE) // defined in line.h
		{
			cursor->row++;
			cursor->column = 0;
			move(cursor->row, cursor->column);
			//scroll_window(screen, cursor);
		}
	}
}
