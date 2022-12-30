#include <ncurses.h>
#include "window.h"
#include "cursor.h"

#define KEY_ESCAPE 27

enum mode_e
{
	VISUAL,
	EDIT
};

static void draw_bottom(
		const enum mode_e mode,
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
	move(cursor->row, cursor->column);

	refresh();
}

int main()
{
	initscr();
	clear();
	noecho();
	cbreak();

	struct cursor_t cursor;
	cursor_init(&cursor);

	struct window_t window;
	window_init(&window);

	enum mode_e mode = VISUAL;

	draw_bottom(mode, &window, &cursor);

	while(1)
	{
		int key_pressed = getch();
		switch (key_pressed)
		{
			case 'i':
				if (mode == VISUAL)
				{
					mode = EDIT;
					draw_bottom(mode, &window, &cursor);
				}
				break;

			case KEY_ESCAPE: // technically this code is also for ALT
				if (mode == EDIT)
				{
					mode = VISUAL;
					draw_bottom(mode, &window, &cursor);
				}
				break;

			default:
				break;
		}
	}

	clrtoeol();
	endwin();

	return 0;
}

