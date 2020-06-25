#ifndef JFG_INPUT_H
#define JFG_INPUT_H

#include "prelude.h"

enum Input_Event_Type
{
	INPUT_EVENT_TYPE_MOUSE_MOVE,
	INPUT_EVENT_TYPE_MOUSE_WHEEL,
	INPUT_EVENT_TYPE_MOUSE_BUTTON,
};

enum Input_Button
{
	INPUT_BUTTON_MOUSE_LEFT,
	INPUT_BUTTON_MOUSE_MIDDLE,
	INPUT_BUTTON_MOUSE_RIGHT,
	INPUT_BUTTON_F1,
	INPUT_BUTTON_F5,

	NUM_INPUT_BUTTONS,
};

enum Input_Button_Flag
{
	INPUT_BUTTON_FLAG_STARTED_DOWN = 1 << 0,
	INPUT_BUTTON_FLAG_ENDED_DOWN   = 1 << 1,
	INPUT_BUTTON_FLAG_HELD_DOWN    = 1 << 2,
};

struct Input_Button_Frame_Data
{
	u16 flags;
	u16 num_transitions;
};

struct Input_Event
{
	Input_Event_Type type;
	union {
		struct {
			i32 dx, dy;
			u32 state_flags;
		} mouse_move;
		struct {
			i32 delta;
		} mouse_wheel;
		struct {
			u32 num_transitions;
		} mouse_button;
	};
};

#define INPUT_MAX_EVENTS 32

struct Input
{
	v2_u32                  mouse_pos;
	v2_i32                  mouse_delta;
	i32                     mouse_wheel_delta;
	Input_Button_Frame_Data button_data[NUM_INPUT_BUTTONS];
	u32         len;
	Input_Event event[INPUT_MAX_EVENTS];

	u32 num_presses(Input_Button button)
	{
		u32 num_presses = button_data[button].num_transitions
		                + (1 - (button_data[button].flags & INPUT_BUTTON_FLAG_ENDED_DOWN));
		return num_presses / 2;
	}
};

void input_reset(Input* input);
void input_push(Input* input, Input_Event event);
void input_button_down(Input* input, Input_Button button);
void input_button_up(Input* input, Input_Button button);
u32 input_get_num_down_transitions(Input* input, Input_Button button);
u32 input_get_num_up_transitions(Input* input, Input_Button button);

#ifndef JFG_HEADER_ONLY

void input_reset(Input* input)
{
	input->len = 0;
	input->mouse_wheel_delta = 0;
	for (u32 i = 0; i < NUM_INPUT_BUTTONS; ++i) {
		Input_Button_Frame_Data *button = &input->button_data[i];
		u16 new_flags = 0;
		if (button->flags & (INPUT_BUTTON_FLAG_ENDED_DOWN | INPUT_BUTTON_FLAG_HELD_DOWN)) {
			new_flags |= INPUT_BUTTON_FLAG_STARTED_DOWN | INPUT_BUTTON_FLAG_HELD_DOWN;
		}
		button->flags = new_flags;
		button->num_transitions = 0;
	}
}

void input_button_down(Input* input, Input_Button button)
{
	Input_Button_Frame_Data *b = &input->button_data[button];
	b->flags |= INPUT_BUTTON_FLAG_ENDED_DOWN;
	++b->num_transitions;
}

void input_button_up(Input* input, Input_Button button)
{
	Input_Button_Frame_Data *b = &input->button_data[button];
	b->flags &= ~(INPUT_BUTTON_FLAG_ENDED_DOWN | INPUT_BUTTON_FLAG_HELD_DOWN);
	++b->num_transitions;
}

void input_push(Input* input, Input_Event event)
{
	ASSERT(input->len < INPUT_MAX_EVENTS);
	input->event[input->len++] = event;
}

u32 input_get_num_down_transitions(Input* input, Input_Button button)
{
	Input_Button_Frame_Data *button_data = &input->button_data[button];
	u32 transitions = button_data->num_transitions;
	if (!(button_data->flags & INPUT_BUTTON_FLAG_STARTED_DOWN)) {
		++transitions;
	}
	return transitions / 2;
}

u32 input_get_num_up_transitions(Input* input, Input_Button button)
{
	Input_Button_Frame_Data *button_data = &input->button_data[button];
	u32 transitions = button_data->num_transitions;
	if (button_data->flags & INPUT_BUTTON_FLAG_STARTED_DOWN) {
		++transitions;
	}
	return transitions / 2;
}

#endif

#endif
