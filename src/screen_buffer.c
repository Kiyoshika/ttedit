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
	screen->max_occupied_line = 1;
	screen->total_lines = 1;
	if (!screen->lines)
		return -1;
	return 0;
}

void screen_draw(
		const struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	size_t buffer_idx = 0;
	char number[10] = {0};
	cursor->line_num_size = log10(screen->max_occupied_line) + 1;

	for (size_t i = screen->start_idx; i < screen->end_idx - 1; ++i)
	{
		move(buffer_idx, 0);
		clrtoeol();
		
		// print line numbers dynamically according to their size
		if (i < screen->max_occupied_line)
		{
			sprintf(number, "%ld", i + 1);
			size_t bound = cursor->line_num_size - strlen(number);
			for (size_t i = 1; i < bound; ++i)
				printw(" ");
			printw("%s", number);
			memset(number, 0, 10);
		}

		if (strlen(screen->lines[i]) == 0 && i < screen->max_occupied_line)
			printw("");
		else if (i >= screen->max_occupied_line)
			printw("~");
		else
		{
			move(buffer_idx, cursor->line_num_size + 1);
			printw("%s", screen->lines[i]);
		}

		buffer_idx++;
	}

	// restore original cursor position
	move(screen->current_line, cursor->column + cursor->line_num_size + 1);

	refresh();
}

void screen_draw_line(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor)
{
	move(screen->current_line, cursor->line_num_size + 1);
	clrtoeol();
	printw("%s", screen->lines[cursor->row]);
	refresh();
}

void screen_scroll_up(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	if (screen->start_idx > 0)
	{
		screen->start_idx--;
		screen->end_idx--;
	}

	screen_draw(screen, cursor);
}

void screen_scroll_down(
		struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	screen->start_idx++;
	screen->end_idx++;
	screen_draw(screen, cursor);
}

void screen_free(
		struct screen_buffer_t* screen)
{
	free(screen->lines);
	screen->lines = NULL;
}

bool screen_read_file(
		struct screen_buffer_t* const screen,
		const char* filename)
{
	FILE* ifile = fopen(filename, "ab+");
	if (!ifile)
		return false;

	char current_buffer[LINE_BUFF_SIZE] = {0};
	size_t current_line = 0;
	while (fgets(current_buffer, LINE_BUFF_SIZE, ifile))
	{
		// replace newlines with terminators (they're written manually
		// in the write file)
		if (current_buffer[strlen(current_buffer)-1] == '\n')
			current_buffer[strlen(current_buffer)-1] = '\0';

		memcpy(
				screen->lines[current_line++], 
				current_buffer, 
				strlen(current_buffer));

		if (current_line == screen->total_lines)
		{
			void* alloc = realloc(screen->lines, screen->total_lines * 2 * sizeof(*screen->lines));
			if (!alloc)
				return false;

			screen->lines = alloc;
			screen->total_lines *= 2;
			for (size_t i = current_line; i < screen->total_lines; ++i)
				memset(screen->lines[i], 0, LINE_BUFF_SIZE);
		}
		screen->max_occupied_line++;
		memset(current_buffer, 0, LINE_BUFF_SIZE);
	}

	fclose(ifile);
	return true;
}

bool screen_write_to_file(
		const struct screen_buffer_t* const screen,
		const char* filename)
{
	FILE* ofile = fopen(filename, "w");
	if (!ofile)
		return false;

	for (size_t i = 0; i < screen->max_occupied_line; ++i)
	{
		fwrite(screen->lines[i], sizeof(char), strlen(screen->lines[i]), ofile);
		// don't add extra newline at end of file
		if (i < screen->max_occupied_line - 1)
			fwrite("\n", sizeof(char), 1, ofile);
	}

	fclose(ofile);
	
	return true;
}
