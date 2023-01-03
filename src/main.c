#include <ncurses.h>
#include "window.h"
#include "cursor.h"
#include "screen_buffer.h"
#include "edit_mode.h"
#include "screen_buffer.h"
#include "command_buffer.h"

#define KEY_ESCAPE 27
#define KEY_TAB 9

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
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);

	refresh();
}

int main()
{
	initscr();
	clear();
	noecho();
	cbreak();

	// use colours if terminal supports it
	if (has_colors())
	{
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		attron(COLOR_PAIR(1));
	}

	// TODO: create an app_t struct that wraps the window, screen and cursor together
	
	struct cursor_t cursor;
	cursor_init(&cursor);

	struct window_t window;
	window_init(&window);

	struct screen_buffer_t screen;
	screen_init(&screen, &window);
	screen_draw(&screen, &cursor);

	struct command_buffer_t command;
	command_init(&command);

	enum mode_e mode = VISUAL;

	draw_bottom(mode, &screen, &window, &cursor);

	while(1)
	{
		// note about these case statements: some of of these cases
		// intentionally leave out the "break" statement to fall back to the "default"
		// case which writes the key to the buffer.
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
					goto writekey;
				break;

			// MOVE CURSOR LEFT
			case 'h':
				if (mode == VISUAL)
					cursor_move_left(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR RIGHT
			case 'l':
				if (mode == VISUAL)
					cursor_move_right(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR UP
			case 'k':
				if (mode == VISUAL)
					cursor_move_up(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR DOWN
			// OR TRIGGER JUMP COMMAND (if buffer is non-empty)
			// TODO: add the cursor_char (last argument) later (which will be used for scope jumping)
			// for now just using a placeholder space character
			case 'j':
				if (mode == VISUAL)
				{
					if (strlen(command.buffer) == 0)
						cursor_move_down(&cursor, &screen);
					else
						command_execute(&command, &cursor, &screen, 'j', ' ');
				}
				else
					goto writekey;
				break;

			// PREPEND LINE
			// (jump to first non-space character in buffer and toggle EDIT mode)
			case 'p':
				if (mode == VISUAL)
				{
					cursor_prepend_line(&cursor, &screen);
					mode = EDIT;
					draw_bottom(mode, &screen, &window, &cursor);
				}
				else
					goto writekey;
				break;

			// APPEND LINE
			// (jump to end of buffer and toggle EDIT mode)
			case 'a':
				if (mode == VISUAL)
				{
					cursor_append_line(&cursor, &screen);
					mode = EDIT;
					draw_bottom(mode, &screen, &window, &cursor);
				}
				else
					goto writekey;
				break;

			// ADD NEW LINE UNDER CURSOR AND JUMP TO IT
			case 'n':
				if (mode == VISUAL)
				{
					edit_insert_new_line(&screen, &cursor, false);
					screen_draw(&screen, &cursor);
					mode = EDIT;
					draw_bottom(mode, &screen, &window, &cursor);
				}
				else
					goto writekey;
				break;

			// DELETE CURRENT LINE AND JUMP UP A LINE
			case 'd':
				if (mode == VISUAL)
				{
					edit_delete_current_line(&screen, &cursor);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;

			// JUMP TO BOTTOM OF VISUAL BUFFER (no scrolling)
			case 'b':
				if (mode == VISUAL)
					cursor_jump_visual_bottom(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO BOTTOM OF ENTIRE BUFFER (scrolling if needed)
			case 'B':
				if (mode == VISUAL)
					cursor_jump_bottom(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO TOP OF VISUAL BUFFER (no scrolling)
			case 't':
				if (mode == VISUAL)
					cursor_jump_visual_top(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO TOP OF ENTIRE BUFFER (scrolling if needed)
			case 'T':
				if (mode == VISUAL)
					cursor_jump_top(&cursor, &screen);
				else
					goto writekey;
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (mode == VISUAL)
					command_append(&command, key_pressed);
				else
					goto writekey;
				break;

			// INSERT TAB (4 spaces)
			case KEY_TAB:
				if (mode == EDIT)
				{
					for (size_t i = 0; i < 4; ++i)
						edit_write_key(&screen, &cursor, ' ');
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
				else if (mode == VISUAL)
					command_clear(&command);
				break;

			writekey:
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

