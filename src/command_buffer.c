#include "command_buffer.h"
#include "cursor.h"
#include "screen_buffer.h"

void command_init(
		struct command_buffer_t* const command)
{
	command->buffer_idx = 0;
	memset(command->buffer, 0, COMMAND_BUFF_SIZE);
}

// as of now, this is the exact same as command_init()
// but using a different name for clarity
void command_clear(
		struct command_buffer_t* const command)
{
	command->buffer_idx = 0;
	memset(command->buffer, 0, COMMAND_BUFF_SIZE);
}

void command_append(
		struct command_buffer_t* const command,
		const char ch)
{
	if (command->buffer_idx >= COMMAND_BUFF_SIZE)
		return;

	command->buffer[command->buffer_idx++] = ch;
	command->buffer[command->buffer_idx] = '\0';
}

void command_execute(
		struct command_buffer_t* const command,
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen,
		const char trigger_char,
		const char cursor_char)
{
	// if command buffer is empty, do nothing
	if (strlen(command->buffer) == 0)
		return;

	switch (trigger_char)
	{
		case 'j': // line jumps
		{
			char* endptr = NULL;
			uint32_t line_num = strtoul(command->buffer, &endptr, 10);
			// if command buffer contains noise, just ignore it
			if (strlen(endptr) > 0)
				break;
			else
				cursor_jump_line(cursor, screen, line_num);
		}
		break;
	}

	command_clear(command);
}
