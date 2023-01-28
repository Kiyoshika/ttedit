#include "screen_buffer.h"
#include "window.h"
#include "cursor.h"
#include "color_schemes.h"

// NOTE: keep this list sorted so we can perform a bsearch()
// for better performance
// TODO: have a configuration file for this so we can support arbitrary languages
#define N_KEYWORDS 35
#define MAX_KEYWORD_LEN 20
static char KEYWORD_LIST[N_KEYWORDS][MAX_KEYWORD_LEN] = {
	"#define",
	"#include",
	"break",
	"case",
	"char",
	"const",
	"double",
	"else",
	"enum",
	"for",
	"float",
	"goto",
	"if",
	"int",
	"int8_t",
	"int16_t",
	"int32_t",
	"int64_t",
	"long",
	"register",
	"return",
	"short",
	"sizeof",
	"size_t",
	"static",
	"struct",
	"switch",
	"uint8_t",
	"uint16_t",
	"uint32_t",
	"uint64_t",
	"union",
	"void",
	"volatile",
	"while"
};

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

// tokenise a particular string [token_str] compared
// to tokenizing single characters
static void _tokenize_string(
		const char* line,
		char (*tokens)[DOUBLE_LINE_BUFF_SIZE],
		size_t* n_tokens,
		size_t* token_buff_len,
		size_t* token_buff_idx,
		const char* token_str,
		size_t* token_idx)
{
	size_t token_str_len = strlen(token_str);

	if (*token_idx + token_str_len <= strlen(line) 
			&& strncmp(&line[*token_idx], token_str, token_str_len) == 0)
	{
		for (size_t idx = 0; idx < token_str_len; ++idx)
		{
			(*tokens)[(*token_buff_idx)++] = token_str[idx];
			(*token_buff_len)++;
			(*token_idx)++;
		}
		(*tokens)[(*token_buff_idx)++] = '\0';
		(*token_buff_len)++;

		(*n_tokens)++;
	}
}

void _tokenize_line(
		const char* line,
		char (*tokens)[DOUBLE_LINE_BUFF_SIZE],
		size_t* n_tokens,
		size_t* token_buff_len)
{
	// NOTE: for now I'm hardcoding the single/multi line comment tokens,
	// but later these could be read from a config file to support multiple languages
	size_t token_buff_idx = 0;

	for (size_t i = 0; i < strlen(line); ++i)
	{
		// SINGLE COMMENT
		_tokenize_string(
				line,
				tokens,
				n_tokens,
				token_buff_len,
				&token_buff_idx,
				"//",
				&i);

		// MULTI COMMENT (begin)
		_tokenize_string(
				line,
				tokens,
				n_tokens,
				token_buff_len,
				&token_buff_idx,
				"/*",
				&i);
		
		// MULTI COMMENT (end)
		_tokenize_string(
				line,
				tokens,
				n_tokens,
				token_buff_len,
				&token_buff_idx,
				"*/",
				&i);

		// always tokenise on spaces and punctuation (except for some special cases)
		if (line[i] == ' ' || 
				(ispunct(line[i]) && line[i] != '_' && line[i] != '#'))
		{
			// only write a null terminator at beginning if we
			// have some written text before the space. for example,
			// if we have a comment, those already append a null terminator
			if (token_buff_idx > 0
				&& (*tokens)[token_buff_idx - 1] != '\0')
			{
				(*tokens)[token_buff_idx++] = '\0';
				(*n_tokens)++;
				(*token_buff_len)++;
			}
			(*tokens)[token_buff_idx++] = line[i];
			(*tokens)[token_buff_idx++] = '\0';
			(*n_tokens)++;
			(*token_buff_len) += 2;
		}
		// everything else
		else if (isalnum(line[i]) || ispunct(line[i])) 
		{
			(*tokens)[token_buff_idx++] = line[i];
			(*token_buff_len)++;
		}
	}
}

static int cmp(const void* a, const void* b)
{
	return strcmp((const char*)a, (const char*)b);
}

static void screen_print_token(
		const char* current_token,
		bool* inside_single_comment,
		bool* inside_multi_comment,
		bool* inside_single_quote,
		bool* inside_double_quote)
{
	// note: colour pairs are defined in main.c setup
	
	// COLOUR KEYWORD
	if (!*inside_single_comment
			&& !*inside_multi_comment
			&& !*inside_single_quote
			&& !*inside_double_quote
			&& bsearch(current_token, KEYWORD_LIST, N_KEYWORDS, MAX_KEYWORD_LEN, &cmp))
		attron(COLOR_PAIR(SCHEME_KEYWORD));

	// COLOUR COMMENTS AND QUOTES
	// TODO: generalise this later for future languages - for now I'm just hardcoding them
	else if (strncmp(current_token, "//", 2) == 0)
		*inside_single_comment = true;
	else if (strncmp(current_token, "/*", 2) == 0)
		*inside_multi_comment = true;
	else if (strncmp(current_token, "*/", 2) == 0)
	{
		// we still want to highlight the closing comment with comment colour
		*inside_multi_comment = false;
		attron(COLOR_PAIR(SCHEME_COMMENT));
	}
	else if (strncmp(current_token, "\'", 1) == 0)
	{
		// we still want to highlight the closing quote with quote colour 
		*inside_single_quote = !*inside_single_quote;
		attron(COLOR_PAIR(SCHEME_QUOTE));
	}
	else if (strncmp(current_token, "\"", 1) == 0)
	{
		// we still want to highlight the closing quote with quote colour 
		*inside_double_quote = !*inside_double_quote;
		attron(COLOR_PAIR(SCHEME_QUOTE));
	}

	// comments have higher precedence (e.g., if you have quotes inside a comment,
	// we don't want to mix colours around
	if (*inside_single_comment 
			|| *inside_multi_comment)
		attron(COLOR_PAIR(SCHEME_COMMENT));
	else if (*inside_single_quote
			|| *inside_double_quote)
		attron(COLOR_PAIR(SCHEME_QUOTE));

	printw("%s", current_token);

	attron(COLOR_PAIR(SCHEME_REGULAR));
}

static void screen_print_line(
		const struct screen_buffer_t* const screen,
		struct cursor_t* const cursor,
		const size_t line_num,
		bool* inside_single_comment,
		bool* inside_multi_comment,
		bool* inside_single_quote,
		bool* inside_double_quote)
{
	char tokens[DOUBLE_LINE_BUFF_SIZE] = {0};
	char current_token[DOUBLE_LINE_BUFF_SIZE] = {0};
	size_t n_tokens = 0;
	size_t token_buff_idx = 0;
	size_t current_buff_idx = 0;
	size_t token_buff_len = 0;

	_tokenize_line(screen->lines[line_num], &tokens, &n_tokens, &token_buff_len);

	for (size_t i = 0; i < n_tokens; ++i)
	{
		for (; token_buff_idx < token_buff_len + 1; ++token_buff_idx)
		{
			if (tokens[token_buff_idx] == '\0')
			{
				screen_print_token(
						current_token, 
						inside_single_comment, 
						inside_multi_comment,
						inside_single_quote,
						inside_double_quote);
				memset(current_token, 0, DOUBLE_LINE_BUFF_SIZE);
				current_buff_idx = 0;
				continue;
			}
			else
				current_token[current_buff_idx++] = tokens[token_buff_idx];
		}
	}

	// special case where we haven't written a full token yet
	// (without this, the first text you write doesn't render)
	if (n_tokens == 0)
		printw("%s", tokens);
}

void _highlight_text(
		const struct screen_buffer_t* const screen,
		const struct cursor_t* const cursor)
{
	// TODO: break these conditions into their own functions because this
	// unexpectedly became very long
	attron(COLOR_PAIR(SCHEME_HIGHLIGHT));

	const size_t offset = cursor->line_num_size + 1;
	const size_t row_offset = screen->end_idx - screen->max_rows;

	// start first row relative to cursor
	if (cursor->row == cursor->highlight_row)
	{
		size_t start_idx = min(cursor->column, cursor->highlight_column);
		size_t end_idx = max(cursor->column, cursor->highlight_column);

		for (size_t c = start_idx; c <= end_idx; ++c)
		{
			move(screen->current_line, c + offset);
			printw("%c", screen->lines[cursor->row][c]);
		}
		
	}
	else if (cursor->row > cursor->highlight_row)
	{
		size_t start_row = cursor->highlight_row;
		size_t end_row = cursor->row;

		
		// if highlight_row is within visible frame, then we highlight
		// from the cursor to end of line for the first line, full lines in between,
		// and up to highlight_cursor for last line
		if (cursor->highlight_row >= screen->start_idx
				&& cursor->highlight_row <= screen->end_idx)
		{
			// first row
			for (size_t c = cursor->highlight_column; c < strlen(screen->lines[cursor->highlight_row]); ++c)
			{
				move(cursor->highlight_row - row_offset, c + offset);
				printw("%c", screen->lines[cursor->highlight_row][c]);
			}

			// in-between rows
			for (size_t r = cursor->highlight_row + 1; r < cursor->row; ++r)
			{
				move(cursor->highlight_row + (r - cursor->highlight_row) - row_offset, 0 + offset);
				printw("%s", screen->lines[r]);
			}

			// last row
			for (size_t c = 0; c < cursor->column; ++c)
			{
				move(cursor->row - row_offset, c + offset);
				printw("%c", screen->lines[cursor->row][c]);
			}

		}
		// if highlight_row is OUTSIDE of visible frame, highlight everything up to cursor
		else
		{
			for (size_t row = screen->start_idx; row < cursor->row; ++row)
			{
				move(row - screen->start_idx, 0 + offset);
				printw("%s", screen->lines[row]);
			}

			for (size_t c = 0; c < cursor->column; ++c)
			{
				move(cursor->row - row_offset, c + offset);
				printw("%c", screen->lines[cursor->row][c]);
			}
		}

	}
	else if (cursor->highlight_row > cursor->row)
	{
		// TODO: finish this
	}

	refresh();

	attron(COLOR_PAIR(SCHEME_REGULAR));
}

void screen_draw(
		const struct screen_buffer_t* const screen,
		struct cursor_t* const cursor)
{
	size_t buffer_idx = 0;
	char number[10] = {0};
	cursor->line_num_size = log10(screen->max_occupied_line) + 1;

	bool inside_single_comment = false;
	bool inside_multi_comment = false;
	bool inside_single_quote = false;
	bool inside_double_quote = false;

	for (size_t i = screen->start_idx; i < screen->end_idx - 1; ++i)
	{
		if (inside_multi_comment)
			attron(COLOR_PAIR(SCHEME_COMMENT));
		else
			attron(COLOR_PAIR(SCHEME_REGULAR));

		move(buffer_idx, 0);
		clrtoeol();
		
		// print line numbers dynamically according to their size
		if (i < screen->max_occupied_line)
		{
			attron(COLOR_PAIR(SCHEME_LINE_NUMBER));
			sprintf(number, "%ld", i + 1);
			size_t bound = cursor->line_num_size - strlen(number);
			for (size_t i = 1; i < bound; ++i)
				printw(" ");
			printw("%s", number);
			memset(number, 0, 10);
			attron(COLOR_PAIR(SCHEME_REGULAR));
		}

		if (i >= screen->max_occupied_line)
		{
			attron(COLOR_PAIR(SCHEME_REGULAR));
			printw("~");
		}
		else
		{
			move(buffer_idx, cursor->line_num_size + 1);
			screen_print_line(
					screen, 
					cursor, 
					i,
					&inside_single_comment,
					&inside_multi_comment,
					&inside_single_quote,
					&inside_double_quote);
		}

		buffer_idx++;
		// at the end of a line, we don't want to highlight single comments on the next line
		inside_single_comment = false;
	}

	// if highlighting cursor, repaint region with SCHEME_HIGHLIGHT
	if (cursor->highlight_mode)
		_highlight_text(screen, cursor);

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
		struct cursor_t* const cursor,
		const char* filename)
{
	// attempt to read file
	FILE* ifile = NULL;
	ifile = fopen(filename, "r");
	if (ifile)
	{
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
		cursor->line_num_size = log10(screen->max_occupied_line) + 1;
		fclose(ifile);
	}
	// otherwise, create new blank file
	else
	{
		ifile = fopen(filename, "w+");
		fclose(ifile);
	}

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
