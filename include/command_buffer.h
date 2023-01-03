#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <string.h>
#include <stdlib.h>

#define COMMAND_BUFF_SIZE 10

// forward declaration
struct screen_buffer_t;
struct cursor_t;

struct command_buffer_t
{
	char buffer[COMMAND_BUFF_SIZE];
	size_t buffer_idx;
};

// initialise contents of command buffer
void command_init(
		struct command_buffer_t* const command);

// clear command buffer
void command_clear(
		struct command_buffer_t* const command);

// append a character to the command buffer.
// if buffer is already full, command is ignored
void command_append(
		struct command_buffer_t* const command,
		const char ch);

// execute whatever's in command buffer and clear buffer.
// This function takes a "trigger" character such as 'd'
// or 'j' to indicate deletions or jumps respectively.
// This can also take whatever character the cursor is currently hovering
// at the time of hitting the trigger_char.
// refer to command list in README for details.
void command_execute(
		struct command_buffer_t* const command,
		struct cursor_t* const cursor,
		struct screen_buffer_t* const screen,
		const char trigger_char,
		const char cursor_char);
#endif
