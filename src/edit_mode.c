#include "edit_mode.h"
#include "screen_buffer.h"
#include "cursor.h"

// move all of the screen->lines buffer up one index into
// row_idx + 1 (e.g., after deleting a line or backspacing an entire line)
static void _shift_buffer_up(
		struct screen_buffer_t* const screen,
		const size_t row_idx)
{
	size_t i = row_idx + 1;
	for (; i < screen->total_lines - 1; ++i)
	{
		// clear buffer and copy next line into current line
		memset(screen->lines[i], 0, LINE_BUFF_SIZE);
		memcpy(screen->lines[i], screen->lines[i+1], strlen(screen->lines[i+1]));
		screen->lines[i][strlen(screen->lines[i+1])] = '\0'; // just in case...
	}
}

void edit_write_key(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const int key_pressed)
{
	if (key_pressed == '\n' || key_pressed == KEY_ENTER)
	{
		if (screen->current_line < screen->max_rows)
			screen->current_line++;

		cursor->row++;
		if (cursor->row >= screen->total_lines)
		{
			void* alloc = realloc(screen->lines, screen->total_lines * 2 * sizeof(*screen->lines));
			if (!alloc)
			{
				// TODO: some error
			}

			screen->lines = alloc;
			screen->total_lines *= 2;

			for (size_t i = cursor->row; i < screen->total_lines; ++i)
				memset(screen->lines[i], 0, sizeof(*screen->lines));
		}

		if (cursor->row + 1 >= screen->max_occupied_line)
			screen->max_occupied_line++;

		if (screen->current_line == screen->max_rows - 1)
		{
			screen->current_line--;
			screen_scroll_down(screen, cursor);
		}

		cursor->column = 0;
		move(screen->current_line, cursor->column);
	}
	else if (key_pressed == KEY_BACKSPACE || key_pressed == 127)
	{
		// remove current character and shift all elements from the right over
		// to the left
		char* buffer = screen->lines[cursor->row];

		// if current line buffer is empty, wrap up to previous row
		if (strlen(buffer) == 0)
		{
			// if at very top of buffer, do nothing
			if (cursor->row == 0)
				return;

			if (cursor->row > 0)
				cursor->row--;

			if (screen->current_line > 0)
				screen->current_line--;

			// when removing a line, we also decrement the max occupied lines
			// and shift the buffer up one
			screen->max_occupied_line--;
			_shift_buffer_up(screen, cursor->row);

			cursor->column = strlen(screen->lines[cursor->row]);
			move(screen->current_line, cursor->column);

			refresh();

			return;
		}

		size_t i = cursor->column > 0 ? cursor->column - 1 : 0;
		for (; i < strlen(buffer) - 1; ++i)
			buffer[i] = buffer[i+1];
		buffer[i] = '\0';

		if (cursor->column > 0)
			cursor->column--;

		screen_draw_line(screen, cursor);
		//screen_draw(screen, cursor);
		move(screen->current_line, cursor->column);
	}
	else
	{
		// write key to buffer and wrap cursor if necessary
		screen->lines[cursor->row][cursor->column++] = key_pressed;
		screen_draw_line(screen, cursor);
		//screen_draw(screen, cursor);
		if (cursor->column == LINE_BUFF_SIZE) // defined in screen_buffer.h
		{
			cursor->row++;
			cursor->column = 0;
			screen_scroll_down(screen, cursor);
			move(screen->current_line, cursor->column);
		}
	}
}
