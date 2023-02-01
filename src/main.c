#include <ncurses.h>
#include <stdio.h>
#include <signal.h>
#include "color_schemes.h"
#include "cursor.h"
#include "screen_buffer.h"
#include "edit_mode.h"
#include "screen_buffer.h"
#include "command_buffer.h"

#define KEY_ESCAPE 27
#define KEY_TAB 9

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	// do nothing (prevent user closing with Ctrl+C)
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Please provide a file name.\n");
		return -1;
	}

	// disable Ctrl+C (otherwise we can't free memory from screen buffer properly)
	signal(SIGINT, INThandler);

	char* filename = argv[1];

	initscr();
	clear();
	noecho();
	cbreak();

	// use colours if terminal supports it
	if (has_colors())
	{
		start_color();
		init_pair(SCHEME_REGULAR, COLOR_WHITE, COLOR_BLACK);
		init_pair(SCHEME_KEYWORD, COLOR_CYAN, COLOR_BLACK);
		init_pair(SCHEME_LINE_NUMBER, COLOR_YELLOW, COLOR_BLACK);
		init_pair(SCHEME_HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);
		// for now comment and quotes will have the same scheme,
		// but eventually when we can let users customise them, they
		// can change if wanted
		init_pair(SCHEME_QUOTE, COLOR_GREEN, COLOR_BLACK);
		init_pair(SCHEME_COMMENT, COLOR_GREEN, COLOR_BLACK);
	}

	// TODO: create an app_t struct that wraps the window, screen and cursor together
	
	struct cursor_t cursor;
	cursor_init(&cursor);

	struct screen_buffer_t screen;
	screen_init(&screen);
	
	struct command_buffer_t command;
	command_init(&command);

	if (!screen_read_file(&screen, &cursor, filename))
	{
		fprintf(stderr, "There was a problem opening the file.\n");
		screen_free(&screen);
		endwin();
		return -1;
	}
	screen_draw(&screen, &cursor);


	screen_draw_bottom(&screen, &cursor, &command);

	while(1)
	{
		int key_pressed = getch();
		switch (key_pressed)
		{
			// COPY BUFFER
			case 'C':
				if (screen.mode == VISUAL && cursor.highlight_mode)
					edit_copy_buffer(&screen, &cursor);
				else if (screen.mode == EDIT)
					goto writekey;
				break;
			// PASTE BUFFER
			case 'P':
				if (screen.mode == VISUAL)
					edit_paste_buffer(&screen, &cursor);
				else
					goto writekey;
				break;
			// HIGHLIGHT TEXT
			case 'H':
				if (screen.mode == VISUAL)
				{
					cursor_toggle_highlight(&cursor);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;
			// JUMP SCOPES
			case 'J':
				if (screen.mode == VISUAL)
					cursor_jump_scope(&cursor, &screen, screen.lines[cursor.row][cursor.column]);
				else
					goto writekey;
				break;
			// WRITE CONTENTS TO FILE
			case 's':
				if (screen.mode == VISUAL)
				{
					if (!screen_write_to_file(&screen, filename))
					{
						fprintf(stderr, "Couldn't write to file");
						screen_free(&screen);
						endwin();
						return -1;
					}
					// TODO: write "save successful" or some type of message
				}
				else
					goto writekey;
				break;

			// QUIT FILE
			case 'q':
				if (screen.mode == VISUAL)
				{
					screen_free(&screen);
					endwin();
					return 0;
				}
				else
					goto writekey;
				break;

			// SWITCH TO EDIT MODE
			case 'i':
				if (screen.mode == VISUAL)
				{
					screen.mode = EDIT;
					screen_draw_bottom(&screen, &cursor, &command);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;

			// MOVE CURSOR LEFT
			case 'h':
				if (screen.mode == VISUAL)
					cursor_move_left(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR RIGHT
			case 'l':
				if (screen.mode == VISUAL)
					cursor_move_right(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR UP
			case 'k':
				if (screen.mode == VISUAL)
					cursor_move_up(&cursor, &screen);
				else
					goto writekey;
				break;

			// MOVE CURSOR DOWN
			// OR TRIGGER JUMP COMMAND (if buffer is non-empty)
			case 'j':
				if (screen.mode == VISUAL)
				{
					if (strlen(command.buffer) == 0)
						cursor_move_down(&cursor, &screen);
					else
					{
						command_execute(&command, &cursor, &screen, 'j', ' ');
						screen_draw_bottom(&screen, &cursor, &command);
					}
				}
				else
					goto writekey;
				break;

			// PREPEND LINE
			// (jump to first non-space character in buffer and toggle EDIT mode)
			case 'p':
				if (screen.mode == VISUAL)
				{
					cursor_prepend_line(&cursor, &screen);
					screen.mode = EDIT;
					screen_draw_bottom(&screen, &cursor, &command);
				}
				else
					goto writekey;
				break;

			// APPEND LINE
			// (jump to end of buffer and toggle EDIT mode)
			case 'a':
				if (screen.mode == VISUAL)
				{
					cursor_append_line(&cursor, &screen);
					screen.mode = EDIT;
					screen_draw_bottom(&screen, &cursor, &command);
				}
				else
					goto writekey;
				break;

			// ADD NEW LINE UNDER CURSOR AND JUMP TO IT
			case 'n':
				if (screen.mode == VISUAL)
				{
					edit_insert_new_line(&screen, &cursor, false);
					screen_draw(&screen, &cursor);
					screen.mode = EDIT;
					screen_draw_bottom(&screen, &cursor, &command);
				}
				else
					goto writekey;
				break;

			// DELETE CURRENT LINE AND JUMP UP A LINE
			case 'd':
				if (screen.mode == VISUAL)
				{
					edit_delete_current_line(&screen, &cursor);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;

			// JUMP TO BOTTOM OF VISUAL BUFFER (no scrolling)
			case 'b':
				if (screen.mode == VISUAL)
					cursor_jump_visual_bottom(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO BOTTOM OF ENTIRE BUFFER (scrolling if needed)
			case 'B':
				if (screen.mode == VISUAL)
					cursor_jump_bottom(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO TOP OF VISUAL BUFFER (no scrolling)
			case 't':
				if (screen.mode == VISUAL)
					cursor_jump_visual_top(&cursor, &screen);
				else
					goto writekey;
				break;

			// JUMP TO TOP OF ENTIRE BUFFER (scrolling if needed)
			case 'T':
				if (screen.mode == VISUAL)
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
				if (screen.mode == VISUAL)
				{
					command_append(&command, key_pressed);
					screen_draw_bottom(&screen, &cursor, &command);
				}
				else
					goto writekey;
				break;

			// INSERT TAB (4 spaces)
			case KEY_TAB:
				if (screen.mode == EDIT)
				{
					for (size_t i = 0; i < 4; ++i)
						edit_write_key(&screen, &cursor, ' ');
					screen_draw(&screen, &cursor);
				}
				break;

			// SWITCH TO VISUAL MODE
			case KEY_ESCAPE: // technically this code is also for ALT
				if (screen.mode == EDIT)
				{
					screen.mode = VISUAL;
					screen_draw_bottom(&screen, &cursor, &command);
					screen_draw(&screen, &cursor);
				}
				else if (screen.mode == VISUAL)
				{
					command_clear(&command);
					if (cursor.highlight_mode)
					{
						cursor_toggle_highlight(&cursor);
						screen_draw(&screen, &cursor);
					}
					screen_draw_bottom(&screen, &cursor, &command);
				}
				break;

			// JUMP FORWARD A WORD
			case 'w':
				if (screen.mode == VISUAL)
				{
					cursor_jump_word_forward(&cursor, &screen);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;

			// JUMP BACKWARD A WORD
			case 'W':
				if (screen.mode == VISUAL)
				{
					cursor_jump_word_backward(&cursor, &screen);
					screen_draw(&screen, &cursor);
				}
				else
					goto writekey;
				break;

			writekey:
			default:
				// WRITE TO BUFFER IN EDIT MODE
				if (screen.mode == EDIT)
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

