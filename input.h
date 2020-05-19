#ifndef JFG_INPUT_H
#define JFG_INPUT_H

#include "prelude.h"

enum Input_Event_Type
{
	INPUT_EVENT_TYPE_MOUSE_MOVE,
};

struct Input_Event
{
	Input_Event_Type type;
	union {
		struct {
			i32 dx, dy;
		} mouse_move;
	};
};

#define MAX_INPUT_EVENTS 32

struct Input
{
	u32 len;
	Input_Event event[MAX_INPUT_EVENTS];
};

#ifndef JFG_HEADER_ONLY
// TODO -- input translation for different platforms
#endif

#endif
