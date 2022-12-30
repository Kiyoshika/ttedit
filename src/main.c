#include <ncurses.h>
#include "window.h"
#include "cursor.h"
#include "screen_buffer.h"
#include "edit_mode.h"
#include "screen_buffer.h"

#define KEY_ESCAPE 27

enum mode_e
{
	VISUAL,
	EDIT
};

static void draw_bottom(
		const enum mode_e mode,
		const struct screen_buffer_t* const screen,
		const struct window_t* const window,
		const struct cursor_t* const cursor)
{
	// clear buffer before writing
	move(window->rows - 1, 0);
	clrtoeol();

	// since we already moved the cursor, we can use regular printw
	switch (mode)
	{
		case VISUAL:
			printw("MODE: VISUAL");
			break;
		case EDIT:
			printw("MODE: EDIT");
			break;
	}

	// restore original cursor position
	move(screen->current_line, cursor->column);

	refresh();
}

int main()
{
	initscr();
	clear();
	noecho();
	cbreak();

	// TODO: create an app_t struct that wraps the window, screen and cursor together
	
	struct cursor_t cursor;
	cursor_init(&cursor);

	struct window_t window;
	window_init(&window);

	struct screen_buffer_t screen;
	screen_init(&screen, &window);
	screen_draw(&screen, &cursor);

	enum mode_e mode = VISUAL;

	draw_bottom(mode, &screen, &window, &cursor);

	while(1)
	{
		int key_pressed = getch();
		switch (key_pressed)
		{
			// SWITCH TO EDIT MODE
			case 'i':
				if (mode == VISUAL)
				{
					mode = EDIT;
					draw_bottom(mode, &screen, &window, &cursor);
					screen_draw(&screen, &cursor);
				}
				else
				{
					edit_write_key(&screen, &cursor, key_pressed);
					screen_draw(&screen, &cursor);
				}
				break;

			// SWITCH TO VISUAL MODE
			case KEY_ESCAPE: // technically this code is also for ALT
				if (mode == EDIT)
				{
					mode = VISUAL;
					draw_bottom(mode, &screen, &window, &cursor);
					screen_draw(&screen, &cursor);
				}
				break;
			// DUMP OUT CURRENT SCREEN BUFFER FOR DEBUGGING
			case 'd':
				if (mode == VISUAL)
				{
					clear();
					for (size_t i = 0; i < screen.total_lines; ++i)
					{
						move(i, 0);
						printw("%s", screen.lines[i]);
					}
				}

			default:
				// WRITE TO BUFFER IN EDIT MODE
				if (mode == EDIT)
				{
					edit_write_key(&screen, &cursor, key_pressed);
					screen_draw(&screen, &cursor);
				}

				break;
		}
	}

	endwin();

	return 0;
}

