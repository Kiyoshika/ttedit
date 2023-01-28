#include "edit_mode.h"
#include "screen_buffer.h"
#include "cursor.h"

// move all of the screen->lines buffer up one index into
// row_idx + 1 (e.g., after deleting a line or backspacing an entire line)
void edit_shift_buffer_up(
		struct screen_buffer_t* const screen,
		const size_t row_idx)
{
	size_t i = row_idx;
	for (; i < screen->total_lines; ++i)
	{
		// clear buffer and copy next line into current line
		memset(screen->lines[i], 0, LINE_BUFF_SIZE);
		memcpy(screen->lines[i], screen->lines[i+1], strlen(screen->lines[i+1]));
		screen->lines[i][strlen(screen->lines[i+1])] = '\0'; // just in case...
	}
}

// move all of the screen->lines buffer down one index
// starting from the bottom to row_idx + 1 (e.g., after pressing return key on a line)
void edit_shift_buffer_down(
		struct screen_buffer_t* const screen,
		const size_t row_idx)
{
	// NOTE: since max_occupied_line actually starts at 1 rather than 0, we subtract
	// an extra one from the indices to account for that. Admittedly it's a little messy
	// but it is what it is
	for (size_t i = screen->max_occupied_line - 1; i > row_idx; --i)
	{
		memset(screen->lines[i+1-1], 0, LINE_BUFF_SIZE);
		memcpy(screen->lines[i+1-1], screen->lines[i-1], strlen(screen->lines[i-1]));
		screen->lines[i+1-1][strlen(screen->lines[i-1])] = '\0'; // just in case
	}
}

void edit_backspace(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	// if cursor is a very top of the buffer, do nothing
	if (cursor->row == 0 && cursor->column == 0)
		return;

	// remove current character and shift all elements from the right over
	// to the left
	char buffer[LINE_BUFF_SIZE] = {0};
	memcpy(buffer, screen->lines[cursor->row], strlen(screen->lines[cursor->row]));

	// if current line buffer is empty, wrap up to previous row
	if (strlen(buffer) == 0)
	{
		// when removing a line, we also decrement the max occupied lines
		// and shift the buffer up one
		screen->max_occupied_line--;
		edit_shift_buffer_up(screen, cursor->row);

		if (cursor->row > 0)
			cursor->row--;

		if (screen->current_line > 0)
			screen->current_line--;
		cursor->column = strlen(screen->lines[cursor->row]);
		move(screen->current_line, cursor->column);

		refresh();

		return;
	}
	// if cursor is at beginning but buffer is not empty,
	// we want to take the buffer and wrap it up to the previous line
	else if (cursor->column == 0 && strlen(buffer) > 0)
	{
		screen->max_occupied_line--;
		edit_shift_buffer_up(screen, cursor->row);

		if (cursor->row > 0)
			cursor->row--;
		
		if (screen->current_line > 0)
			screen->current_line--;

		size_t row_len = strlen(screen->lines[cursor->row]);
		size_t remaining_len = LINE_BUFF_SIZE - row_len - 1;
		strncat(screen->lines[cursor->row], buffer, remaining_len);

		cursor->column = row_len;
		move(screen->current_line, cursor->column);

		return;
	}

	size_t i = cursor->column > 0 ? cursor->column - 1 : 0;
	for (; i < strlen(buffer) - 1; ++i)
		screen->lines[cursor->row][i] = screen->lines[cursor->row][i+1];
	screen->lines[cursor->row][i] = '\0';

	if (cursor->column > 0)
		cursor->column--;

	screen_draw_line(screen, cursor);
	//screen_draw(screen, cursor);
	move(screen->current_line, cursor->column);
}

void edit_insert_new_line(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const bool copy_buffer)
{
	char buffer[LINE_BUFF_SIZE] = {0};
	memcpy(buffer, screen->lines[cursor->row], strlen(screen->lines[cursor->row]));

	if (screen->current_line < screen->max_rows)
		screen->current_line++;

	cursor->row++;
	screen->max_occupied_line++;
	if (screen->max_occupied_line + 1 >= screen->total_lines)
	{
		void* alloc = realloc(screen->lines, screen->total_lines * 2 * sizeof(*screen->lines));
		if (!alloc)
		{
			// TODO: some error
		}

		screen->lines = alloc;
		screen->total_lines *= 2;
	}

	if (screen->current_line == screen->max_rows - 1)
	{
		screen->current_line--;
		screen_scroll_down(screen, cursor);
	}

	// if buffer is not empty, copy contents from cursor
	// to end of buffer and paste it to next line (while removing
	// it from original line)
	char partial_buffer[LINE_BUFF_SIZE] = {0};
	if (copy_buffer)
	{
		memcpy(partial_buffer, buffer + cursor->column, strlen(buffer) - cursor->column);
		memset(screen->lines[cursor->row - 1] + cursor->column, 0, LINE_BUFF_SIZE - cursor->column);
	}
	edit_shift_buffer_down(screen, cursor->row);
	memset(screen->lines[cursor->row], 0, LINE_BUFF_SIZE);
	if (copy_buffer)
		memcpy(screen->lines[cursor->row], partial_buffer, strlen(partial_buffer));

	cursor->column = 0;
	move(screen->current_line, cursor->column);
	refresh();
}

void edit_delete_current_line(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	edit_shift_buffer_up(screen, cursor->row);
	if (screen->max_occupied_line > 1)
		screen->max_occupied_line--;
	if (cursor->row == screen->max_occupied_line)
	{
		cursor->row--;
		if (screen->current_line > 0)
			screen->current_line--;
	}
	cursor->column = strlen(screen->lines[cursor->row]);
	move(cursor->row, cursor->column);
	refresh();
}

void edit_write_key(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const int key_pressed)
{
	if (key_pressed == '\n' || key_pressed == KEY_ENTER)
		edit_insert_new_line(screen, cursor, true);
	else if (key_pressed == KEY_BACKSPACE || key_pressed == 127)
		edit_backspace(screen, cursor);
	else
	{
		// if cursor is in the middle of a buffer, shift the buffer
		// over to the right to insert the character
		if (strlen(screen->lines[cursor->row]) > 0
				&& cursor->column < strlen(screen->lines[cursor->row])
				&& cursor->column < LINE_BUFF_SIZE - 1)
		{
			for (size_t i = strlen(screen->lines[cursor->row]); i --> cursor->column;)
				screen->lines[cursor->row][i + 1] = screen->lines[cursor->row][i];
		}

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

void edit_copy_buffer(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	size_t n_rows = 0;
	if (cursor->row > cursor->highlight_row)
		n_rows = cursor->row - cursor->highlight_row;
	else
		n_rows = cursor->highlight_row - cursor->row;
	n_rows++;

	char (*copy_buffer)[LINE_BUFF_SIZE] = calloc(n_rows, sizeof(*copy_buffer));

	// TODO: need some type of error here incase the allocation fails

	size_t start_row = min(cursor->row, cursor->highlight_row);
	size_t start_column = min(cursor->column, cursor->highlight_column);

	size_t end_row = max(cursor->row, cursor->highlight_row);
	size_t end_column = max(cursor->column, cursor->highlight_column);

	size_t copied_rows = 0;

	// copy first row (need to handle the cursor position appropriately)
	if (n_rows == 1)
	{
		for (size_t c = start_column; c <= end_column; ++c)
			strncat(copy_buffer[copied_rows], &screen->lines[start_row][c], 1);
	}
	else
	{
		if (cursor->row > cursor->highlight_row)
			for (size_t c = cursor->highlight_column; c < strlen(screen->lines[start_row]); ++c)
				strncat(copy_buffer[copied_rows], &screen->lines[start_row][c], 1);
		else
			for (size_t c = cursor->column; c < strlen(screen->lines[start_row]); ++c)
				strncat(copy_buffer[copied_rows], &screen->lines[start_row][c], 1);
	}
	copied_rows++;

	// copy middle rows (entire lines)
	for (size_t r = start_row + 1; r < end_row; ++r)
		strncpy(copy_buffer[copied_rows++], screen->lines[r], strlen(screen->lines[r]));

	// copy last row (need to handle the cursor position appropriately)
	if (cursor->row > cursor->highlight_row)
		for (size_t c = 0; c < cursor->column; ++c)
			strncat(copy_buffer[copied_rows], &screen->lines[end_row][c], 1);
	else
		for (size_t c = 0; c < cursor->highlight_column; ++c)
			strncat(copy_buffer[copied_rows], &screen->lines[end_row][c], 1);

	free(screen->copy_buffer);
	screen->copy_buffer = copy_buffer;
	screen->copy_buffer_rows = n_rows;

	cursor_toggle_highlight(cursor);
	screen_draw(screen, cursor);
}
