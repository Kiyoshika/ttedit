#include "line_array.h"
#include "line.h"

struct line_array_t* line_array_create(
#include "line.h"
		size_t capacity)
{
	struct line_array_t* line_array = malloc(sizeof(struct line_array_t));
	if (!line_array)
		return NULL;

	line_array->line = malloc(sizeof(struct line_t) * capacity);
	if (!line_array->line)
	{
		free(line_array);
		return NULL;
	}

	line_array->current_line = 0;
	line_array->capacity = capacity;

	return line_array;
}

int8_t line_array_add(
		struct line_array_t* const line_array,
		const struct line_t* const line)
{
	memcpy(
			&line_array->line[line_array->current_line++],
			line,
			sizeof(struct line_t));

	if (line_array->current_line == line_array->capacity)
	{
		void* alloc = realloc(line_array->line, line_array->capacity * 2 * sizeof(struct line_t));
		if (!alloc)
			return -1;
		
		line_array->line = alloc;
		line_array->capacity *= 2;
	}

	return 0;
}

void line_array_free(
		struct line_array_t* line_array)
{
	// since the inner contents are memcpy'd we don't
	// need to individually free them
	free(line_array->line);
	line_array->line = NULL;

	free(line_array);
	line_array = NULL;
}
