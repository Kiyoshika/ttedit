#include "cursor.h"

void cursor_init(
		struct cursor_t* const cursor)
{
	cursor->row = 0;
	cursor->column = 0;
}
