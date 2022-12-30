#include "screen_buffer.h"
#include "window.h"
#include "cursor.h"

int8_t screen_init(
		struct screen_buffer_t* const screen,
		const struct window_t* const window)
{
	screen->start_idx = 0;
	screen->end_idx = window->rows;
	screen->max_rows = window->rows;
	screen->current_line = 0;
	screen->lines = calloc(1, sizeof(*screen->lines));
	screen->total_lines = 1;
	if (!screen->lines)
		return -1;
	return 0;
}

void screen_draw(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor)
{
	size_t buffer_idx = 0;
	for (size_t i = screen->start_idx; i < screen->end_idx - 1; ++i)
	{
		move(buffer_idx++, 0);
		clrtoeol();
		if (strlen(screen->lines[i]) == 0)
			printw("~");
		else
			printw("%s", screen->lines[i]);
	}

	// restore original cursor position
	move(screen->current_line, cursor->column);

	refresh();
}

void screen_draw_line(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor)
{
	move(screen->current_line, 0);
	clrtoeol();
	printw("%s", screen->lines[cursor->row]);
	refresh();
}

void screen_free(
		struct screen_buffer_t* screen)
{
	free(screen->lines);
	screen->lines = NULL;
}
