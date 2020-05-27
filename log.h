#ifndef JFG_LOG_H
#define JFG_LOG_H

#include "prelude.h"

#define LOG_MAX_LINE_LENGTH 160
#define LOG_MAX_LINES       4096
#define LOG_BUFFER_SIZE (LOG_MAX_LINE_LENGTH * LOG_MAX_LINES)

struct Log
{
	char  buffer[LOG_BUFFER_SIZE];
	char *line_starts[LOG_MAX_LINES];
	u32   cur_line;
	u32   buffer_pos;
};

void  log(Log* l, char* string);
char* log_get_line(Log* l, u32 line);

#ifndef JFG_HEADER_ONLY
void log(Log* l, char* string)
{
	u32 cur_length = 0;
	u32 lines_written = 0;
	u32 buffer_pos = l->buffer_pos;
	if (LOG_BUFFER_SIZE - buffer_pos < LOG_MAX_LINE_LENGTH) {
		buffer_pos = 0;
	}
	char *buffer = l->buffer;
	char *line_start = &buffer[buffer_pos];
	for (char *p = string; *p; ++p) {
		++cur_length;
		buffer[buffer_pos] = *p;
		buffer_pos = (buffer_pos + 1) % LOG_BUFFER_SIZE;
		if (cur_length == LOG_MAX_LINE_LENGTH - 1) {
			cur_length = 0;
			l->line_starts[l->cur_line++ % LOG_MAX_LINES] = line_start;
			buffer[buffer_pos] = 0;
			buffer_pos = (buffer_pos + 1) % LOG_BUFFER_SIZE;
			line_start = &buffer[buffer_pos];
		}
	}
	if (cur_length) {
		l->line_starts[l->cur_line++ % LOG_MAX_LINES] = line_start;
		buffer[buffer_pos] = 0;
		buffer_pos = (buffer_pos + 1) % LOG_BUFFER_SIZE;
	}
	l->buffer_pos = buffer_pos;
}

char* log_get_line(Log* l, u32 line)
{
	u32 cur_line = l->cur_line;
	if (line >= cur_line || (cur_line > LOG_MAX_LINES && line < cur_line - LOG_MAX_LINES)) {
		return NULL;
	}
	return l->line_starts[line % LOG_MAX_LINES];
}
#endif

#endif
