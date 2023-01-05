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

	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
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

	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
	refresh();
}

void cursor_move_left(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen)
{
	if (cursor->column > 0)
		cursor->column--;

	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
	refresh();
}

void cursor_move_right(
		struct cursor_t* const cursor,
		const struct screen_buffer_t* const screen)
{
	if (cursor->column < strlen(screen->lines[cursor->row]))
		cursor->column++;

	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
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
			move(screen->current_line, cursor->column);
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
	move(screen->current_line, cursor->column);
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
	cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
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
	cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
	refresh();
	screen_draw(screen, cursor);
}

void cursor_jump_visual_top(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	cursor->row -= screen->current_line;
	screen->current_line = 0;
	cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
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
	cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);
	refresh();
	screen_draw(screen, cursor);
}

void cursor_jump_line(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen,
		const uint32_t line_num)
{
	// this portion is if the jump is within the visible screen buffer
	// OR if jumping to a cursor position ABOVE the visible screen buffer
	// which will make the jumped line the top of the buffer (scrolling up)
	if (line_num < screen->end_idx - 1
			&& line_num <= screen->max_occupied_line)
	{
		cursor->row = line_num - 1;
		if (line_num < screen->start_idx)
		{
			screen->start_idx = line_num - 1;
			screen->end_idx = screen->start_idx + screen->max_rows;
			screen->current_line = 0;
		}
		else
			screen->current_line = cursor->row - screen->start_idx;
		cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
		move(screen->current_line, cursor->column + cursor->line_num_size + 1);
	}
	// this portion is if the jump is BELOW the visible screen buffer causing us
	// to scroll down
	else if (line_num >= screen->end_idx - 1 && line_num < screen->max_occupied_line)
	{
		screen->end_idx = line_num + 1;
		screen->start_idx = screen->end_idx - screen->max_rows;
		screen->current_line = screen->max_rows - 2;
		cursor->row = line_num - 1;
		cursor->column = cursor->column > strlen(screen->lines[cursor->row]) ? strlen(screen->lines[cursor->row]) : cursor->column;
		move(screen->current_line, cursor->column + cursor->line_num_size + 1);
		refresh();
		screen_draw(screen, cursor);
	}

	screen_draw(screen, cursor);
}

void cursor_jump_word_forward(
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen)
{
	bool found_word = false;
	size_t row = cursor->row;
	size_t column = cursor->column;
	while (!found_word && row < screen->max_occupied_line)
	{
		for (size_t i = column; i < strlen(screen->lines[row]); ++i)
		{
			// start of new line, after a space character, or after a punctuation character
			// disallowing consecutive space (e.g., tab) or punctuation (e.g., ->)
			if ((i == 0 && row != cursor->row)
				|| ((screen->lines[row][i] == ' ' && screen->lines[row][i+1] != ' ') && !ispunct(screen->lines[row][i+1]))
				|| (ispunct(screen->lines[row][i]) && !ispunct(screen->lines[row][i+1]) && screen->lines[row][i] != ' '))
			{
				found_word = true;
				// scroll screen by one line if needed
				screen->current_line += row - cursor->row;
				if (screen->current_line >= screen->max_rows - 1)
				{
					screen->start_idx++;
					screen->end_idx++;
					screen->current_line--;
				}
				cursor->row = row;
				cursor->column = i;
				// if word is beginning of buffer, leave it at 0, otherwise move
				// cursor after space/punctuation character
				if (i > 0)
					cursor->column++;
				move(screen->current_line, cursor->column);
				break;
			}
		}

		row++;
		column = 0;
	}
}
